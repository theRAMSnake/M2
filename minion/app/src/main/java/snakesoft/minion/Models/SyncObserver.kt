package snakesoft.minion.Models

import java.util.*

class SyncObserver
{
    val OnUpdated = Event1<String>()

    fun beginSync(name: String)
    {
        OnUpdated("Started: $name")
    }

    fun endSync()
    {
        OnUpdated("Finished")
    }

    fun itemsModified(items: Int)
    {
        OnUpdated("Updated: $items")
    }

    fun itemLoaded(itemsCount: Int)
    {
        OnUpdated("Loaded: $itemsCount")
    }

    fun finish()
    {
        OnUpdated("Sync finished successfully")
    }

    fun itemDetailsUpdated(id: UUID)
    {
        OnUpdated("Updated: $id")
    }

    fun itemDetailsUpdated(id: String)
    {
        OnUpdated("Updated: $id")
    }
}
