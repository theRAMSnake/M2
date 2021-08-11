package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json

class InboxServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    fun update(items: List<String>)
    {
        for(x in items)
        {
            val jsonData = "{\"operation\": \"push\", \"listId\":\"inbox\", \"value\": \"$x\"}"
            mMateriaConnection.sendMessage(jsonData)
        }
    }
}
