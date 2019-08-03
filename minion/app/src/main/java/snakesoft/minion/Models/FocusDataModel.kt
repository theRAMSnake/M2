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
        @Serializable(with = UUIDSerializer::class)
        var graphId: UUID,
        @Serializable(with = UUIDSerializer::class)
        var objId: UUID,
        var text: String,
        var completed: Boolean
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

        var numModified = 0
        Items.forEach {
            if(it.completed)
            {
                val toSend = Strategy.FocusItemInfo.newBuilder()
                        .setId(toProto(it.id))
                        .setDetails(Strategy.GraphObjectId.newBuilder()
                                .setGraphId(toProto(it.graphId))
                                .setObjectId(toProto(it.objId))
                                .build())
                        .build()

                proxy.completeFocusItem(toSend)
                numModified++
            }
        }

        observer.itemsModified(numModified)


        val rawItems = queryFocusItems(proxy)
        val graphToNodesMap = rawItems.groupBy ({ it.details.graphId }, { it })

        val newItems = mutableListOf<FocusItem>()

        graphToNodesMap.forEach {
            val g = proxy.loadGraph(it.key)

            observer.OnUpdated("Graph loaded: ${it.key}")

            it.value.forEach {
                val node = g.nodesList.find { n -> n.id.objectId == it.details.objectId }
                if(node != null)
                {
                    newItems.add(FocusItem(
                            UUID.fromString(it.id.guid),
                            UUID.fromString(it.details.graphId.guid),
                            UUID.fromString(it.details.objectId.guid),
                            node.attrs.brief,
                            false))
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

        Db.put("FocusItems2", json)
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = Json.parse(FocusItem.serializer().list, Db["FocusItems2"])
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

    fun toggleItem(id: UUID) {
        val item = Items.find { it.id == id }!!

        item.completed = !item.completed
    }

    fun isItemToggled(id: UUID): Boolean {
        val item = Items.find { it.id == id }!!

        return item.completed
    }
}
