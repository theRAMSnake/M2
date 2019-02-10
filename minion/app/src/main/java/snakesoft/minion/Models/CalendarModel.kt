package snakesoft.minion.Models

import calendar.Calendar
import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import java.util.*

import kotlinx.serialization.*
import kotlinx.serialization.json.Json
import snakesoft.minion.materia.toProto

@Serializable
data class CalendarItem(
        val id: java.util.UUID,
        var text: String,
        var timestamp: Long,
        var trackingInfo: StatusOfChange = StatusOfChange.None
    )

class CalendarModel(private val Db: LocalDatabase)
{
    private var Items: MutableMap<java.util.UUID, CalendarItem> = mutableMapOf()
    val AllItems: List<CalendarItem>
        get() = Items.values.toList()

    init
    {
        loadState()
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

        val queried = queryAllItems(proxy)

        Items = queried.associate{ it.id to it }.toMutableMap()

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
        val json = Json.stringify(CalendarItem.serializer().list, Items.values.toList())

        Db.put("CalendarItems", json)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: CalendarServiceProxy): List<CalendarItem>
    {
        val threeYears: Long = 94670778

        val result = mutableListOf<CalendarItem>()

        val queryResult = proxy.query(Calendar.TimeRange.newBuilder().setTimestampFrom(System.currentTimeMillis() / 1000 - threeYears).
                setTimestampTo(System.currentTimeMillis() / 1000 + threeYears).build())

        for(x in queryResult.itemsList)
        {
            result.add(CalendarItem(java.util.UUID.fromString(x.id.guid), x.text, x.timestamp))
        }

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = Json.parse(CalendarItem.serializer().list, Db["CalendarItems"]).associate { it.id to it }.toMutableMap()
        }
        catch(ex: Exception)
        {

        }
    }

    fun clear()
    {
        Items.clear()
    }
}
