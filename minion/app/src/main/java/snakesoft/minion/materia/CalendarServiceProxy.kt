package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import snakesoft.minion.Models.ITrackable
import snakesoft.minion.Models.StatusOfChange
import snakesoft.minion.Models.UUIDSerializer

@Serializable
data class CalendarItem(
        @Serializable(with = UUIDSerializer::class)
        override var id: java.util.UUID,
        var text: String,
        var timestamp: Long,
        var nodeReference: String = "",
        var typename: String = "calendar_item",
        var reccurencyTypeChoice: String = "None",
        var entityTypeChoice: String = "Task",
        var urgencyChoice: String = "Not Urgent",
        override var trackingInfo: StatusOfChange = StatusOfChange.None
) : ITrackable

@Serializable
data class CalendarParams(
        var text: String,
        var timestamp: Long,
        var reccurencyTypeChoice: String,
        var entityTypeChoice: String,
        var urgencyChoice: String
)

@Serializable
data class QueryResult(val id: String, val typename: String, val object_list: List<CalendarItem>)

@Serializable
data class CalendarModify(val operation: String = "modify", val id: String, val params: CalendarParams)

@Serializable
data class CalendarAdd(val operation: String, val typename: String, var params: CalendarParams)

class CalendarServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(MateriaUnreachableException::class)
    fun query(): List<CalendarItem>
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(calendar_item)\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        return format.decodeFromString(QueryResult.serializer(), resp).object_list
    }

    @Throws(MateriaUnreachableException::class)
    fun completeItem(id: java.util.UUID)
    {
        val jsonData = "{\"operation\": \"complete\", \"id\": \"${id}\"}"
        mMateriaConnection.sendMessage(jsonData)
    }

    @Throws(MateriaUnreachableException::class)
    fun addItem(item: CalendarItem)
    {
        val cp = CalendarParams(item.text, item.timestamp, item.reccurencyTypeChoice, item.entityTypeChoice, item.urgencyChoice)
        var ca = CalendarAdd("create", "calendar_item", cp)
        mMateriaConnection.sendMessage(Json.encodeToString(CalendarAdd.serializer(), ca))
    }

    @Throws(MateriaUnreachableException::class)
    fun editItem(item: CalendarItem): Boolean
    {
        val cp = CalendarParams(item.text, item.timestamp, item.reccurencyTypeChoice, item.entityTypeChoice, item.urgencyChoice)
        var ca = CalendarModify("modify", item.id.toString(), cp)
        mMateriaConnection.sendMessage(Json.encodeToString(CalendarModify.serializer(), ca))
        return true
    }
}
