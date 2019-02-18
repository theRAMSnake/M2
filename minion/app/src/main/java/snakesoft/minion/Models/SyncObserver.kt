package snakesoft.minion.Models

import java.util.Vector

data class SyncedEntity(val name: String) {
    var changed = 0
    var added = 0
    var loaded = 0
    var deleted = 0
}

class SyncObserver
{
    val OnFinished = Event()

    private val SyncedEntities = mutableListOf<SyncedEntity>()
    private var SyncedEntity = SyncedEntity("")

    val Log: String
        get() {
            val b = StringBuilder()

            for (en in SyncedEntities) {
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

    fun beginSync(name: String)
    {
        SyncedEntity = SyncedEntity(name)
    }

    fun endSync()
    {
        SyncedEntities.add(SyncedEntity)
        SyncedEntity = SyncedEntity("")
    }

    fun itemChanged()
    {
        SyncedEntity.changed++
    }

    fun itemDeleted()
    {
        SyncedEntity.deleted++
    }

    fun itemAdded()
    {
        SyncedEntity.added++
    }

    fun itemLoaded(itemsCount: Int)
    {
        SyncedEntity.loaded += itemsCount
    }

    fun finish()
    {
        OnFinished()
    }
}
