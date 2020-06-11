package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.JSON
import kotlinx.serialization.json.Json
import kotlinx.serialization.list

@Serializable
data class Query(val operation: String = "query", val ids: List<String>)

@Serializable
data class Object(val id: String, val typename: String, val objects: List<String>)

@Serializable
data class ListParams(val objects: List<String>)

@Serializable
data class Modify(val operation: String = "modify", val id: String, val params: ListParams)

@Serializable
data class ObjectList(val id: String, val typename: String, val object_list: List<Object>)

class InboxServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    fun update(items: List<String>)
    {
        val jsonData = JSON.stringify(Query.serializer(), Query("query", listOf("inbox")))
        val inbox = JSON.parse(ObjectList.serializer(), mMateriaConnection.sendMessage(jsonData)).object_list[0]

        var md = Modify("modify", inbox.id, ListParams(inbox.objects + items))
        val jsonDataM = JSON.stringify(Modify.serializer(), md)
        mMateriaConnection.sendMessage(jsonDataM)
    }
}
