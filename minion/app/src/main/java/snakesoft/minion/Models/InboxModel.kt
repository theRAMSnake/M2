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
        for (i in Items)
        {
            when(i.trackingInfo)
            {
                StatusOfChange.Edit ->
                {
                    proxy.editItem(toProto(i))
                    numModified++
                }
                StatusOfChange.Delete ->
                {
                    proxy.deleteItem(toProto(i.id))
                    numModified++
                }
                StatusOfChange.Add ->
                {
                    proxy.addItem(toProto(i))
                    numModified++
                }
                else -> {}
            }
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
        val json = Json.stringify(InboxItem.serializer().list, Items.toList())

        Db.put("InboxItems", json)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: InboxServiceProxy): List<InboxItem>
    {
        val result = mutableListOf<InboxItem>()

        val queryResult = proxy.loadInbox()

        for(x in queryResult.itemsList)
        {
            result.add(InboxItem(java.util.UUID.fromString(x.id.guid), x.text))
        }

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = TrackedCollection(Json.parse(InboxItem.serializer().list, Db["InboxItems"]))
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
