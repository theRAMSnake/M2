package snakesoft.minion.Models

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import kotlinx.serialization.list
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import snakesoft.minion.materia.StrategyServiceProxy
import snakesoft.minion.materia.toProto
import strategy.Strategy
import java.util.*

@Serializable
data class ResourceItem(
        @Serializable(with = UUIDSerializer::class)
        var id: java.util.UUID,
        var name: String,
        var baseValue: Int,
        var delta: Int
)

class StrategyModel(private val Db: LocalDatabase)
{
    private var Items = listOf<ResourceItem>()

    val Ids: List<UUID>
        get() = Items.map { it.id }

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Strategy")

        val proxy = StrategyServiceProxy(connection)

        val queried = queryAllItems(proxy)

        var numModifications = 0
        for (i in Items)
        {
            var queriedItem = queried.find { it.id == i.id }
            if(i.delta != 0 && queriedItem != null)
            {
                i.baseValue = queriedItem.baseValue
                updateResource(i, proxy)
                numModifications++
            }
        }

        observer.itemsModified(numModifications)

        Items = queryAllItems(proxy)//Query again since our last modification

        observer.itemLoaded(Items.size)

        saveState()

        observer.endSync()
    }

    private fun saveState()
    {
        val json = Json.stringify(ResourceItem.serializer().list, Items.toList())

        Db.put("StrategyResources", json)
    }

    private fun updateResource(item: ResourceItem, proxy: StrategyServiceProxy)
    {
        proxy.modifyResource(toProto(item))
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: StrategyServiceProxy): List<ResourceItem>
    {
        val result = mutableListOf<ResourceItem>()

        val queryResult = proxy.loadResources()

        for(x in queryResult.itemsList)
        {
            result.add(ResourceItem(java.util.UUID.fromString(x.id.guid), x.name, x.value, 0))
        }

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = Json.parse(ResourceItem.serializer().list, Db["StrategyResources"])
        }
        catch(ex: Exception)
        {

        }
    }

    fun clear()
    {
        Items = listOf<ResourceItem>()
        saveState()
    }

    fun getItemName(item: UUID): String
    {
        return Items.find { it.id == item }!!.name
    }

    fun getItemValue(item: UUID): Int
    {
        val item = Items.find { it.id == item }!!

        return item.baseValue + item.delta
    }

    fun decItem(item: UUID)
    {
        Items.find { it.id == item }!!.delta--
        saveState()
    }

    fun incItem(item: UUID)
    {
        Items.find { it.id == item }!!.delta++
        saveState()
    }
}