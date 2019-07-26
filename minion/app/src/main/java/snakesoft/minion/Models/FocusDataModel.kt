package snakesoft.minion.Models

import common.Common
import kotlinx.serialization.*
import kotlinx.serialization.json.Json
import snakesoft.minion.materia.*
import strategy.Strategy
import java.util.*

@Serializable
data class FocusItem(
        @Serializable(with = UUIDSerializer::class)
        var id: UUID,
        var text: String
)

class FocusDataModel(private val Db: LocalDatabase)
{
    var Items = listOf<FocusItem>()

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Focus")

        val proxy = StrategyServiceProxy(connection)

        val rawItems = queryFocusItems(proxy)
        val nodeToItemsMap = rawItems.groupBy { it.details.objectId }
        val graphToNodesMap = rawItems.groupBy ({ it.details.graphId }, { it.details.objectId })

        val newItems = mutableListOf<FocusItem>()

        graphToNodesMap.forEach {
            val g = proxy.loadGraph(it.key)

            observer.OnUpdated("Graph loaded: ${it.key}")

            it.value.forEach {
                val node = g.nodesList.find { n -> n.id.objectId == it }
                if(node != null)
                {
                    val text = when(val numItems = nodeToItemsMap.getOrElse(node.id.objectId) { listOf() }.size)
                    {
                        1 -> node.attrs.brief
                        else -> node.attrs.brief + " ($numItems times)"
                    }
                    newItems.add(FocusItem(UUID.fromString(node.id.objectId.guid), text))
                }
            }
        }

        Items = newItems

        observer.itemLoaded(Items.size)

        saveState()

        observer.endSync()
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryFocusItems(proxy: StrategyServiceProxy): List<Strategy.FocusItemInfo>
    {
        val result = mutableListOf<Strategy.FocusItemInfo>()

        val queryResult = proxy.loadFocusItems()

        for(x in queryResult.itemsList)
        {
            result.add(x)
        }

        return result
    }

    private fun saveState()
    {
        val json = Json.stringify(FocusItem.serializer().list, Items.toList())

        Db.put("FocusItems", json)
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = Json.parse(FocusItem.serializer().list, Db["FocusItems"])
        }
        catch(ex: Exception)
        {

        }
    }

    fun clear()
    {
        Items = listOf()
        saveState()
    }

    fun getItemName(id: UUID): String
    {
        val item = Items.find { it.id == id }!!

        return item.text
    }
}
