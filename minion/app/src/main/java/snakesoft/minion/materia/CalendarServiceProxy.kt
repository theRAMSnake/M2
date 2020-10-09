package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import calendar.Calendar
import common.Common
import kotlinx.serialization.Optional
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.JSON
import snakesoft.minion.Models.ITrackable
import snakesoft.minion.Models.StatusOfChange
import snakesoft.minion.Models.UUIDSerializer

@Serializable
data class CalendarItem(
        @Serializable(with = UUIDSerializer::class)
        override var id: java.util.UUID,
        var text: String,
        var timestamp: Long,
        var reccurencyType: Int = 0,
        var entityType: Int = 0,
        @Optional
        var nodeReference: String = "",
        var typename: String = "calendar_item",
        @Optional
        override var trackingInfo: StatusOfChange = StatusOfChange.None
) : ITrackable

@Serializable
data class CalendarParams(
        var text: String,
        var timestamp: Long,
        var reccurencyType: Int = 0,
        var entityType: Int = 0
)

@Serializable
data class QueryResult(val id: String, val typename: String, val object_list: List<CalendarItem>)

@Serializable
data class CalendarModify(val operation: String = "modify", val id: String, val params: CalendarParams)

@Serializable
data class CalendarAdd(val operation: String = "create", val typename: String = "calendar_item", var params: CalendarParams)

class CalendarServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun query(): List<CalendarItem>
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(calendar_item)\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        println(resp)
        return JSON.parse(QueryResult.serializer(), resp).object_list
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun completeItem(id: java.util.UUID)
    {
        val jsonData = "{\"operation\": \"complete\", \"id\": \"${id}\"}"
        mMateriaConnection.sendMessage(jsonData)
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addItem(item: CalendarItem)
    {
        val cp = CalendarParams(item.text, item.timestamp, item.reccurencyType, item.entityType)
        var ca = CalendarAdd("create", "calendar_item", cp)
        mMateriaConnection.sendMessage(JSON.stringify(CalendarAdd.serializer(), ca))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun editItem(item: CalendarItem): Boolean
    {
        val cp = CalendarParams(item.text, item.timestamp, item.reccurencyType, item.entityType)
        var ca = CalendarModify("modify", item.id.toString(), cp)
        mMateriaConnection.sendMessage(JSON.stringify(CalendarModify.serializer(), ca))
        return true
    }
}
