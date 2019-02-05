package snakesoft.minion.Models

import java.util.Vector

internal class SyncedEntity(var name: String) {
    var changed = 0
    var added = 0
    var loaded = 0
    var deleted = 0
}

class SyncObserver {

    private val mSyncedEntities = Vector<SyncedEntity>()
    private var mSyncedEntity: SyncedEntity? = null

    val log: String
        get() {
            val b = StringBuilder()

            for (en in mSyncedEntities) {
                b.append(en.name)
                b.append(System.lineSeparator())

                b.append("+")
                b.append(en.added)

                b.append("-")
                b.append(en.deleted)

                b.append("=")
                b.append(en.changed)

                b.append("L")
                b.append(en.loaded)

                b.append(System.lineSeparator())
                b.append(System.lineSeparator())
            }

            return b.toString()
        }

    fun beginSync(name: String) {
        mSyncedEntity = SyncedEntity(name)
    }

    fun endSync() {
        if (mSyncedEntity != null) {
            mSyncedEntities.add(mSyncedEntity)
        }
    }

    fun itemChanged() {
        mSyncedEntity!!.changed++
    }

    fun itemDeleted() {
        mSyncedEntity!!.deleted++
    }

    fun itemAdded() {
        mSyncedEntity!!.added++
    }

    fun itemLoaded(itemsCount: Int) {
        mSyncedEntity!!.loaded += itemsCount
    }
}
