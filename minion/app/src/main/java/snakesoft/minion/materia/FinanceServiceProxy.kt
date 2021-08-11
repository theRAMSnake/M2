package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import snakesoft.minion.Models.FinanceCategory
import snakesoft.minion.Models.FinanceEvent

@Serializable
data class FinanceCatResult(val id: String, val typename: String, val object_list: List<FinanceCategory>)

@Serializable
data class FinanceAdd(val operation: String = "create", val typename: String = "finance_event", var params: FinanceEvent)

class FinanceServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(MateriaUnreachableException::class)
    fun loadCategories(): List<FinanceCategory>
    {
        val jsonData = "{\"operation\": \"query\", \"filter\": \"IS(finance_category)\"}"
        val resp = mMateriaConnection.sendMessage(jsonData)
        return format.decodeFromString(FinanceCatResult.serializer(), resp).object_list
    }

    @Throws(MateriaUnreachableException::class)
    fun addEvent(item: FinanceEvent)
    {
        var ca = FinanceAdd(params = item)
        mMateriaConnection.sendMessage(Json.encodeToString(FinanceAdd.serializer(), ca))
    }
}
