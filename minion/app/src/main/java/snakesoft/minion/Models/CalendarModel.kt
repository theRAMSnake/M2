package snakesoft.minion.Models

import com.google.protobuf.InvalidProtocolBufferException

import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream
import java.util.ArrayList
import java.util.Vector

import calendar.Calendar
import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.MateriaUnreachableException

/**
 * Created by snake on 11/24/17.
 */

class CalendarModel(private val mProxy: CalendarServiceProxy) {

    val allItems: List<Calendar.CalendarItem>
        get() = ArrayList(mItems!!.itemsList)

    val newId: String
        get() = Integer.toString(++mLastVirtualId)

    private var mLastVirtualId = 0
    private var mItems: Calendar.CalendarItems? = null
    private var mLocalDb: LocalDatabase? = null
    private val mItemsChanges: Vector<StatusOfChange>

    init {
        mItemsChanges = Vector()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver) {
        try {
            observer.beginSync("Calendar")

            for (i in mItemsChanges.indices) {
                if (mItemsChanges[i].type == StatusOfChange.Type.Edit) {
                    mProxy.editItem(mItems!!.getItems(i))
                    observer.itemChanged()
                } else if (mItemsChanges[i].type == StatusOfChange.Type.Delete) {
                    mProxy.deleteItem(mItems!!.getItems(i).id)
                    observer.itemDeleted()
                } else if (mItemsChanges[i].type == StatusOfChange.Type.Add) {
                    mProxy.addItem(mItems!!.getItems(i))
                    observer.itemAdded()
                }
            }

            mItemsChanges.clear()

            mItems = queryAllItems()
            for (i in 0 until mItems!!.itemsCount) {
                mItemsChanges.addElement(StatusOfChange())
            }

            observer.itemLoaded(mItems!!.itemsCount)
        } catch (ex: InvalidProtocolBufferException) {

        }

        saveState()

        observer.endSync()
    }

    fun resetChanges() {
        mItemsChanges.clear()
    }

    /* Returns values in range [timestampFrom, timestampTo) */
    fun getItems(timestampFrom: Long, timestampTo: Long): List<Calendar.CalendarItem> {
        print(timestampFrom)
        print(timestampTo)
        val result = Vector<Calendar.CalendarItem>()

        for (i in 0 until mItems!!.itemsCount) {
            val curItem = mItems!!.getItems(i)
            if (curItem.timestamp >= timestampFrom && curItem.timestamp < timestampTo) {
                if (mItemsChanges[i].type != StatusOfChange.Type.Delete && mItemsChanges[i].type != StatusOfChange.Type.Junk) {
                    result.add(curItem)
                }
            }
        }

        return result
    }

    fun modifyItem(item: Calendar.CalendarItem) {
        for (i in 0 until mItems!!.itemsCount) {
            if (mItems!!.getItems(i).id.guid == item.id.guid) {
                mItems = Calendar.CalendarItems.newBuilder(mItems).setItems(i, item).build()

                if (mItemsChanges[i].type != StatusOfChange.Type.Add) {
                    val ch = StatusOfChange()
                    ch.type = StatusOfChange.Type.Edit
                    mItemsChanges[i] = ch
                }

                break
            }
        }

        saveState()
    }

    fun deleteItem(guid: String) {
        for (i in 0 until mItems!!.itemsCount) {
            if (mItems!!.getItems(i).id.guid == guid) {
                val newStatus = StatusOfChange()

                newStatus.type = if (mItemsChanges[i].type == StatusOfChange.Type.Add)
                    StatusOfChange.Type.Junk
                else
                    StatusOfChange.Type.Delete

                mItemsChanges[i] = newStatus
                break
            }
        }

        saveState()
    }

    fun addItem(item: Calendar.CalendarItem) {
        mItems = Calendar.CalendarItems.newBuilder(mItems).addItems(item).build()

        val ch = StatusOfChange()
        ch.type = StatusOfChange.Type.Add
        mItemsChanges.add(ch)

        saveState()
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

    fun saveState() {
        mLocalDb!!.put("CalendarItems", mItems!!.toByteArray())

        val bos = ByteArrayOutputStream()

        for (x in mItemsChanges) {
            bos.write(x.type.ordinal)
        }

        mLocalDb!!.put("CalendarItemsStatus", bos.toByteArray())
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
}
