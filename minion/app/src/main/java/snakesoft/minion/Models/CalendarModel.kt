package snakesoft.minion.Models

import calendar.Calendar
import common.Common
import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import kotlinx.serialization.*
import kotlinx.serialization.json.Json
import snakesoft.minion.materia.toProto

@Serializable
data class CalendarItem(
        @Serializable(with = UUIDSerializer::class)
        override var id: java.util.UUID,
        var text: String,
        var timestamp: Long,
        override var trackingInfo: StatusOfChange = StatusOfChange.None
    ) : ITrackable

class CalendarModel(private val Db: LocalDatabase)
{
    var Items = TrackedCollection<CalendarItem>()

    init
    {
        loadState()

        Items.OnChanged += {saveState()}
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Calendar")

        val proxy = CalendarServiceProxy(connection)

        var numModifications = 0
        for (i in Items)
        {
            when(i.trackingInfo)
            {
                StatusOfChange.Edit ->
                {
                    proxy.editItem(toProto(i))
                    numModifications++
                }
                StatusOfChange.Delete ->
                {
                    proxy.completeItem(toProto(i.id))
                    numModifications++
                }
                StatusOfChange.Add ->
                {
                    proxy.addItem(toProto(i))
                    numModifications++
                }
                else -> {}
            }
        }

        observer.itemsModified(numModifications)

        val queried = queryAllItems(proxy)

        Items = TrackedCollection(queried)
        Items.OnChanged += {saveState()}

        observer.itemLoaded(Items.size)

        saveState()

        observer.endSync()
    }

    private fun saveState()
    {
        val json = Json.stringify(CalendarItem.serializer().list, Items.toList())

        Db.put("CalendarItems", json)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: CalendarServiceProxy): List<CalendarItem>
    {
        val threeYears: Long = 94670778

        val result = mutableListOf<CalendarItem>()

        val queryResult = proxy.query(Common.TimeRange.newBuilder().setTimestampFrom(System.currentTimeMillis() / 1000 - threeYears).
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
            Items = TrackedCollection(Json.parse(CalendarItem.serializer().list, Db["CalendarItems"]))
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
