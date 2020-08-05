package snakesoft.minion.Models

import common.Common
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import kotlinx.serialization.list
import snakesoft.minion.Activities.getInvalidId
import snakesoft.minion.materia.JournalServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import java.util.*

@Serializable
data class JournalIndexItem(
        var id: String,
        var parentFolderId: String,
        var title: String,
        val isPage: Boolean,
        var modified: Long,
        var typename: String
)

class JournalModel(private val Db: LocalDatabase)
{
    var Items = listOf<JournalIndexItem>()

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Journal")

        val proxy = JournalServiceProxy(connection)
        val newItems = queryIndex(proxy)

        observer.itemLoaded(newItems.size)

        var count = 0
        for(x in newItems)
        {
            if(x.isPage)
            {
                val oldItem = Items.find{it.id == x.id}
                if(oldItem == null || !Db.contains(x.id.toString()) ||
                        oldItem.modified < x.modified)
                {
                    count++
                    syncPage(x.id, proxy)
                    observer.itemDetailsUpdated(x.id)
                }
            }
        }

        Items = newItems
        saveState()

        observer.endSync()
    }

    private fun syncPage(id: String, proxy: JournalServiceProxy)
    {
        val p = proxy.loadPage(id)
        Db.put(id, p)
    }

    private fun saveState()
    {
        val json = Json.stringify(JournalIndexItem.serializer().list, Items)

        Db.put("JournalIndex", json)

        print("done");
    }

    private fun selector(i: JournalIndexItem): Double = if(i.isPage) 1.0 else 0.0

    @Throws(MateriaUnreachableException::class)
    private fun queryIndex(proxy: JournalServiceProxy): List<JournalIndexItem>
    {
        val result = mutableListOf<JournalIndexItem>()

        val queryResult = proxy.loadIndex()

        for(x in queryResult)
        {
            result.add(x)
        }

        result.sortBy { selector(it) }

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = Json.parse(JournalIndexItem.serializer().list, Db["JournalIndex"])
        }
        catch(ex: Exception)
        {
            print(ex.message)
        }
    }

    fun clear()
    {
        Items = listOf()
    }

    fun loadPage(itemId: UUID): String
    {
        try
        {
            return Db[itemId.toString()]
        }
        catch(ex: Exception)
        {
            return String()
        }
    }
}