package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import snakesoft.minion.Models.JournalIndexItem

val format = Json { isLenient = true; ignoreUnknownKeys = true }

@Serializable
data class JournalIndex(val id: String, val typename: String, val object_list: List<JournalIndexItem>, val connection_list: List<Connection>)

@Serializable
data class JournalContentItem(
        var id: String,
        var content: String,
        var typename: String
)

@Serializable
data class JournalContent(val id: String, val typename: String, val object_list: List<JournalContentItem>, val connection_list: List<Connection>)

class JournalServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(MateriaUnreachableException::class)
    fun loadIndex(): List<JournalIndexItem>
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(journal_header)\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        val parsedResp = format.decodeFromString(JournalIndex.serializer(), resp)

        val pageIds = parsedResp.connection_list.filter { it.type == "Extension" }.map{it.A}

        return parsedResp.object_list.map {
            var other = it
            other.isPage = pageIds.contains(other.id)
            val parentCon = parsedResp.connection_list.filter {d -> d.type == "Hierarchy" && d.B == it.id }
            if(parentCon.isNotEmpty()) {
                other.parentFolderId = parentCon[0].A
            }
            other
        }
    }

    @Throws(MateriaUnreachableException::class)
    fun loadPage(id: String): String
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(journal_content) AND Extends(\\\"${id}\\\")\"}"
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
