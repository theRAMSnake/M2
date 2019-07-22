package snakesoft.minion.Models

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import kotlinx.serialization.list
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import snakesoft.minion.materia.FreeDataServiceProxy
import snakesoft.minion.materia.toProto
import java.util.*

@Serializable
data class FDItem(
        @Serializable(with = UUIDSerializer::class)
        var id: UUID,
        var name: String,
        var baseValue: Int,
        var delta: Int
)

class FreeDataModel(private val Db: LocalDatabase)
{
    private var Items = listOf<FDItem>()

    val Ids: List<UUID>
        get() = Items.map { it.id }

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("FreeData")

        val proxy = FreeDataServiceProxy(connection)

        val queried = queryAllItems(proxy)

        var numModifications = 0
        for (i in Items)
        {
            var queriedItem = queried.find { it.name == i.name }
            if(i.delta != 0 && queriedItem != null)
            {
                updateItem(i, proxy)
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
        val json = Json.stringify(FDItem.serializer().list, Items.toList())

        Db.put("FreeDataBlocks2", json)
    }

    private fun updateItem(item: FDItem, proxy: FreeDataServiceProxy)
    {
        proxy.increment(item.name, item.delta)
    }

    @Throws(MateriaUnreachableException::class)
    private fun queryAllItems(proxy: FreeDataServiceProxy): List<FDItem>
    {
        val result = mutableListOf<FDItem>()

        val queryResult = proxy.get()

        for(x in queryResult.itemsList)
        {
            result.add(FDItem(UUID.randomUUID(), x.name, x.value, 0))
        }

        return result
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Items = Json.parse(FDItem.serializer().list, Db["FreeDataBlocks2"])
        }
        catch(ex: Exception)
        {

        }
    }

    fun clear()
    {
        Items = listOf<FDItem>()
        saveState()
    }

    fun getItemName(id: UUID): String
    {
        val item = Items.find { it.id == id }!!

        return item.name
    }

    fun getItemValue(id: UUID): Int
    {
        val item = Items.find { it.id == id }!!

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