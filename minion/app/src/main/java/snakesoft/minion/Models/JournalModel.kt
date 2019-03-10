package snakesoft.minion.Models

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
        @Serializable(with = UUIDSerializer::class)
        var id: java.util.UUID,
        @Serializable(with = UUIDSerializer::class)
        var folderId: java.util.UUID,
        var title: String,
        val isPage: Boolean,
        var modifiedTimestamp: Long
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
        Items = queryIndex(proxy)

        saveState()

        observer.endSync()
    }

    private fun saveState()
    {
        val json = Json.stringify(JournalIndexItem.serializer().list, Items)

        Db.put("JournalIndex", json)
    }

    private fun selector(i: JournalIndexItem): Double = if(i.isPage) 1.0 else 0.0

    @Throws(MateriaUnreachableException::class)
    private fun queryIndex(proxy: JournalServiceProxy): List<JournalIndexItem>
    {
        val result = mutableListOf<JournalIndexItem>()

        val queryResult = proxy.loadIndex()

        for(x in queryResult.itemsList)
        {
            result.add(JournalIndexItem(
                    java.util.UUID.fromString(x.journalItem.id.guid),
                    if(x.journalItem.folderId.guid.count() != 0) UUID.fromString(x.journalItem.folderId.guid) else getInvalidId(),
                    x.journalItem.title,
                    x.isPage,
                    x.modifiedTimestamp
                    ))
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

        }
    }

    fun clear()
    {
        Items = listOf()
    }
}