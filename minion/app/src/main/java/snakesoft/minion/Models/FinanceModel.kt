package snakesoft.minion.Models

import finance.Finance

import kotlinx.serialization.*
import kotlinx.serialization.json.Json
import snakesoft.minion.Activities.getInvalidId
import snakesoft.minion.materia.*
import java.util.*

@Serializable
data class FinanceCategory(
        @Serializable(with = UUIDSerializer::class)
        var id: java.util.UUID,
        var name: String
)

@Serializable
data class FinanceEvent(
        @Serializable(with = UUIDSerializer::class)
        var eventId: java.util.UUID,
        @Serializable(with = UUIDSerializer::class)
        var categoryId: java.util.UUID,
        var details: String,
        var timestamp: Long,
        var amountCents: Long
)

class FinanceModel(private val Db: LocalDatabase)
{
    var Events = listOf<FinanceEvent>()
    var Categories = listOf<FinanceCategory>()
    private var LastDeleted = FinanceEvent(getInvalidId(), getInvalidId(), "", 0, 0)

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Finance")

        val proxy = FinanceServiceProxy(connection)

        //Push events
        for (i in Events)
        {
            proxy.addEvent(toProto(i))
        }
        observer.itemsModified(Events.size)

        //Clear events
        Events = listOf()

        //Load categories
        Categories = queryCategories(proxy)
        observer.itemLoaded(Categories.size)

        saveState()

        observer.endSync()
    }

    private fun saveState()
    {
        var json = Json.stringify(FinanceEvent.serializer().list, Events.toList())
        Db.put("FinanceEvents", json)

        json = Json.stringify(FinanceCategory.serializer().list, Categories.toList())
        Db.put("FinanceCategories", json)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryCategories(proxy: FinanceServiceProxy): List<FinanceCategory>
    {
        val result = mutableListOf<FinanceCategory>()

        val queryResult = proxy.loadCategories()

        for(x in queryResult.itemsList)
        {
            result.add(FinanceCategory(java.util.UUID.fromString(x.id.guid), x.name))
        }

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Events = Json.parse(FinanceEvent.serializer().list, Db["FinanceEvents"])
            Categories = Json.parse(FinanceCategory.serializer().list, Db["FinanceCategories"])
        }
        catch(ex: Exception)
        {

        }
    }

    fun clear()
    {
        Events = listOf()
        Categories = listOf()
    }

    fun addEvent(item: FinanceEvent)
    {
        Events = Events + listOf(item)
        saveState()
    }

    fun deleteEvent(id: UUID)
    {
        LastDeleted = GlobalModel.FinanceModel.Events.first {it.eventId == id}
        Events = Events.filter { it.eventId != id }
        saveState()
    }

    fun restoreLastEvent()
    {
        addEvent(LastDeleted)
    }
}
