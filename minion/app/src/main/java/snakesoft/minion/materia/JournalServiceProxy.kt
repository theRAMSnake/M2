package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import journal.Journal
import common.Common
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.JSON
import snakesoft.minion.Models.FinanceCategory
import snakesoft.minion.Models.JournalIndexItem
import snakesoft.minion.Models.UUIDSerializer

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

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadIndex(): List<JournalIndexItem>
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(journal_header)\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        return JSON.parse(JournalIndex.serializer(), resp).object_list
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun loadPage(id: String): String
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(journal_content) AND .headerId = \\\"${id}\\\"\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        if (resp.contains("object_list\":\"\""))
        {
            return "empty";
        }
        var cnt = JSON.parse(JournalContent.serializer(), resp);
        if(cnt.object_list.isEmpty())
        {
            return "empty";
        }

        return cnt.object_list[0].content
    }
}
