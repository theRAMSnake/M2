package snakesoft.minion.materia

import kotlinx.serialization.Optional
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.JSON
import snakesoft.minion.Models.ITrackable
import snakesoft.minion.Models.StatusOfChange
import snakesoft.minion.Models.UUIDSerializer

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
        return JSON.parse(NFQueryResult.serializer(), resp).object_list[0].content
    }
}
