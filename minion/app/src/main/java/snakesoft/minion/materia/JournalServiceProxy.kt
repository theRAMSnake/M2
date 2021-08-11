package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import snakesoft.minion.Models.JournalIndexItem

val format = Json { isLenient = true; ignoreUnknownKeys = true }

@Serializable
data class JournalIndex(val id: String, val typename: String, val object_list: List<JournalIndexItem>)

@Serializable
data class JournalContentItem(
        var id: String,
        var headerId: String,
        var content: String,
        var typename: String
)

@Serializable
data class JournalContent(val id: String, val typename: String, val object_list: List<JournalContentItem>)

class JournalServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(MateriaUnreachableException::class)
    fun loadIndex(): List<JournalIndexItem>
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(journal_header)\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        return format.decodeFromString(JournalIndex.serializer(), resp).object_list
    }

    @Throws(MateriaUnreachableException::class)
    fun loadPage(id: String): String
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(journal_content) AND .headerId = \\\"${id}\\\"\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        if (resp.contains("object_list\":\"\""))
        {
            return "empty";
        }
        var cnt = format.decodeFromString(JournalContent.serializer(), resp);
        if(cnt.object_list.isEmpty())
        {
            return "empty";
        }

        return cnt.object_list[0].content
    }
}
