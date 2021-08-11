package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json

@Serializable
data class NFObject(val id: String, val typename: String, val content: String)

@Serializable
data class NFQueryResult(val id: String, val typename: String, val object_list: List<NFObject>)

class NewsServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(MateriaUnreachableException::class)
    fun load(): String
    {
        val jsonData = "{\"operation\": \"query\", \"ids\": [\"newsfeed\"]}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        return format.decodeFromString(NFQueryResult.serializer(), resp).object_list[0].content
    }
}
