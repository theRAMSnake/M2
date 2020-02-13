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

@Serializable
data class DetailsToCategoryMapping(
        @Serializable(with = UUIDSerializer::class)
        var id: UUID,
        var details: String
)

class FinanceModel(private val Db: LocalDatabase)
{
    var LastSMSReadDate = System.currentTimeMillis() / 1000 - 60 * 60 * 24 * 7
    var Events = listOf<FinanceEvent>()
    var Categories = listOf<FinanceCategory>()
    private var LastDeleted = FinanceEvent(getInvalidId(), getInvalidId(), "", 0, 0)
    private var DetailsToCategoryMap = listOf<DetailsToCategoryMapping>()

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Finance")

        val proxy = FinanceServiceProxy(connection)
        val newEvents = mutableListOf<FinanceEvent>()

        //Push events
        for (i in Events)
        {
            if(i.categoryId != getInvalidId())
            {
                proxy.addEvent(toProto(i))
            }
            else
            {
                newEvents.add(i)
            }
        }
        observer.itemsModified(Events.size)

        //Clear events
        Events = newEvents

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

        json = Json.stringify(DetailsToCategoryMapping.serializer().list, DetailsToCategoryMap.toList())
        Db.put("DetailsToCategoryMap", json)

        Db.put("LastSMSReadDate", LastSMSReadDate.toString())
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryCategories(proxy: FinanceServiceProxy): List<FinanceCategory>
    {
        val result = mutableListOf<FinanceCategory>()

        val queryResult = proxy.loadCategories()

        for(x in queryResult.itemsList)
        {
            result.add(FinanceCategory(UUID.fromString(x.id.guid), x.name))
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
            LastSMSReadDate = Db["LastSMSReadDate"].toLong()
            DetailsToCategoryMap = Json.parse(DetailsToCategoryMapping.serializer().list, Db["DetailsToCategoryMap"])
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
        var itemWithId = item
        itemWithId.eventId = UUID.randomUUID()
        Events = Events + listOf(itemWithId)
        saveState()
    }

    fun deleteEvent(id: UUID)
    {
        LastDeleted = Events.first {it.eventId == id}
        Events = Events.filter { it.eventId != id }
        saveState()
    }

    fun restoreLastEvent()
    {
        addEvent(LastDeleted)
    }

    fun addPreEvent(date: Long, details: String, amount: Long)
    {
        addEvent(FinanceEvent(getInvalidId(), autodetectCategory(details), details, date, amount))
    }

    private fun autodetectCategory(details: String): UUID
    {
        for(x in DetailsToCategoryMap)
        {
            if(x.details == details)
            {
                return x.id
            }
        }

        return getInvalidId()
    }

    fun updateLastSMSReadDate(date: Long) {
        LastSMSReadDate = if (date > LastSMSReadDate) date else LastSMSReadDate
        saveState()
    }

    fun updateEvent(item: FinanceEvent)
    {
        Events.first {it.eventId == item.eventId}
        Events = Events.filter { it.eventId != item.eventId }
        Events = Events + listOf(item)

        DetailsToCategoryMap = DetailsToCategoryMap.filter {it.details == it.details}
        DetailsToCategoryMap += listOf(DetailsToCategoryMapping(item.categoryId, item.details))

        saveState()
    }
}
