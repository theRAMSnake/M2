package snakesoft.minion.Models

import kotlinx.serialization.*
import kotlinx.serialization.json.Json
import snakesoft.minion.materia.*

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
                    proxy.editItem(i)
                    numModifications++
                }
                StatusOfChange.Delete ->
                {
                    proxy.completeItem(i.id)
                    numModifications++
                }
                StatusOfChange.Add ->
                {
                    proxy.addItem(i)
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
        val json = Json.encodeToString(Items.toList())

        Db.put("CalendarItems", json)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: CalendarServiceProxy): List<CalendarItem>
    {
        return proxy.query()
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = TrackedCollection(Json.decodeFromString(Db["CalendarItems"]))
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
