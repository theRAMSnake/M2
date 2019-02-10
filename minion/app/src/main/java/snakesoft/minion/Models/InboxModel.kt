package snakesoft.minion.Models

import com.google.protobuf.InvalidProtocolBufferException

import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream
import java.util.Vector

import inbox.Inbox
import inbox.Inbox.InboxItems
import snakesoft.minion.materia.InboxServiceProxy
import snakesoft.minion.materia.MateriaUnreachableException

class InboxModel(private val Db: LocalDatabase) {

    val items: List<Inbox.InboxItemInfo>
        get() {
            val result = Vector<Inbox.InboxItemInfo>()

            for (i in 0 until mItems!!.itemsCount) {
                if (mItemsChanges[i].type != StatusOfChange.Type.Delete && mItemsChanges[i].type != StatusOfChange.Type.Junk) {
                    result.add(mItems!!.getItems(i))
                }
            }

            return result
        }

    val newId: String
        get() = Integer.toString(++mLastVirtualId)

    private var mLastVirtualId = 0
    private var mItems: InboxItems? = null
    private var mLocalDb: LocalDatabase? = null
    private val mItemsChanges: Vector<StatusOfChange>

    init {
        mItemsChanges = Vector()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver) {
        try {
            observer.beginSync("Inbox")
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

            mItems = mProxy.inbox
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

    fun modifyItem(item: Inbox.InboxItemInfo) {
        for (i in 0 until mItems!!.itemsCount) {
            if (mItems!!.getItems(i).id.guid == item.id.guid) {
                mItems = InboxItems.newBuilder(mItems).setItems(i, item).build()

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

    fun addItem(item: Inbox.InboxItemInfo) {
        mItems = InboxItems.newBuilder(mItems).addItems(item).build()

        val ch = StatusOfChange()
        ch.type = StatusOfChange.Type.Add
        mItemsChanges.add(ch)

        saveState()
    }

    @Throws(Exception::class)
    fun loadState(localDb: LocalDatabase) {
        mLocalDb = localDb
        try {
            mItems = InboxItems.parseFrom(localDb.get("InboxItems"))
            val byteStream = ByteArrayInputStream(localDb.get("InboxItemsStatus"))
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
        mLocalDb!!.put("InboxItems", mItems!!.toByteArray())

        val bos = ByteArrayOutputStream()

        for (x in mItemsChanges) {
            bos.write(x.type.ordinal)
        }

        mLocalDb!!.put("InboxItemsStatus", bos.toByteArray())
    }


}
