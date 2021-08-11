package snakesoft.minion.Models

import kotlinx.serialization.*
import kotlinx.serialization.json.Json
import snakesoft.minion.materia.*
import android.widget.Toast
import android.content.ClipboardManager.OnPrimaryClipChangedListener
import android.content.Context.CLIPBOARD_SERVICE



@Serializable
data class InboxItem(
        @Serializable(with = UUIDSerializer::class)
        override var id: java.util.UUID,
        var text: String,
        override var trackingInfo: StatusOfChange = StatusOfChange.None
) : ITrackable

class InboxModel(private val Db: LocalDatabase)
{
    var Items = TrackedCollection<InboxItem>()

    init
    {
        loadState()

        Items.OnChanged += {saveState()}
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Inbox")

        val proxy = InboxServiceProxy(connection)

        var numModified = 0
        val lst = mutableListOf<String>()
        for (i in Items)
        {
            when(i.trackingInfo)
            {
                StatusOfChange.Add ->
                {
                    lst.add(i.text)
                    numModified++
                }
                else -> {}
            }
        }

        if(lst.size > 0)
        {
            proxy.update(lst)
        }

        observer.itemsModified(numModified)

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

        Db.put("InboxItems", json)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: InboxServiceProxy): List<InboxItem>
    {
        val result = mutableListOf<InboxItem>()

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = TrackedCollection(Json.decodeFromString(Db["InboxItems"]))
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
