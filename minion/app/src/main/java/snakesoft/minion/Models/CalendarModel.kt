package snakesoft.minion.Models

import com.google.protobuf.InvalidProtocolBufferException

import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream

import calendar.Calendar
import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import java.util.*

data class CalendarItem(
        val id: java.util.UUID,
        var text: String,
        var timestamp: Long,
        var trackingInfo: StatusOfChange = StatusOfChange.None
    )

class CalendarModel(private val Db: LocalDatabase)
{
    private var Items: MutableMap<java.util.UUID, CalendarItem> = mutableMapOf()

    init
    {

    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Calendar")

        val proxy = CalendarServiceProxy(connection)

        for (i in Items)
        {
            when(i.value.trackingInfo)
            {
                StatusOfChange.Edit ->
                {
                    proxy.editItem(toProto(i.value))
                    observer.itemChanged()
                }
                StatusOfChange.Delete ->
                {
                    proxy.deleteItem(toProto(i.key))
                    observer.itemDeleted()
                }
                StatusOfChange.Add ->
                {
                    proxy.addItem(toProto(i.value))
                    observer.itemAdded()
                }
            }
        }

        Items = queryAllItems()

        observer.itemLoaded(Items.size)

        saveState()

        observer.endSync()
    }

    fun replaceItem(item: CalendarItem)
    {
        var newItem = item

        if(newItem.trackingInfo != StatusOfChange.Add)
        {
            newItem.trackingInfo = StatusOfChange.Edit
        }

        Items[newItem.id] = newItem

        saveState()
    }

    fun deleteItem(id: java.util.UUID)
    {
        Items[id]!!.trackingInfo = StatusOfChange.Delete

        saveState()
    }

    fun addItem(item: CalendarItem)
    {
        val newItem = CalendarItem(UUID.randomUUID(), item.text, item.timestamp, StatusOfChange.Add)

        Items[newItem.id] = newItem

        saveState()
    }

    private fun saveState()
    {
        val bos = ByteArrayOutputStream()

        //save here

        Db.put("CalendarItems", bos.toByteArray())
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(): Calendar.CalendarItems {
        val threeYears: Long = 94670778

        try {
            return mProxy.query(Calendar.TimeRange.newBuilder().setTimestampFrom(System.currentTimeMillis() / 1000 - threeYears).setTimestampTo(System.currentTimeMillis() / 1000 + threeYears).build())
        } catch (e: InvalidProtocolBufferException) {
            e.printStackTrace()
            return Calendar.CalendarItems.newBuilder().build()
        }

    }

    @Throws(Exception::class)
    fun loadState(localDb: LocalDatabase) {
        mLocalDb = localDb
        try {
            mItems = Calendar.CalendarItems.parseFrom(localDb.get("CalendarItems"))
            val byteStream = ByteArrayInputStream(localDb.get("CalendarItemsStatus"))
            var next = byteStream.read()
            while (next != -1) {
                val ch = StatusOfChange()
                ch.type = StatusOfChange.Type.values()[next]
                mItemsChanges.add(ch)

                next = byteStream.read()
            }

            assert(mItemsChanges.size == mItems!!.itemsCount)

            for (i in 0 until mItems!!.itemsCount) {
                if (mItems!!.getItems(i).id.guid.length < 10) {
                    val curVirtualId = Integer.parseInt(mItems!!.getItems(i).id.guid)
                    if (curVirtualId > mLastVirtualId) {
                        mLastVirtualId = curVirtualId
                    }
                }
            }
        } catch (ex: InvalidProtocolBufferException) {

        } catch (ex: NullPointerException) {
            if (mItems == null) {
                mItems = Calendar.CalendarItems.newBuilder().build()
            }

            //no data in db case
            if (mItems!!.itemsCount != mItemsChanges.size) {
                for (i in 0 until mItems!!.itemsCount) {
                    mItemsChanges.addElement(StatusOfChange())
                }
            }
        }

        //db check
        if (mItems!!.itemsCount != mItemsChanges.size) {
            throw Exception("Inconsistent DB!")
        }
    }
}
