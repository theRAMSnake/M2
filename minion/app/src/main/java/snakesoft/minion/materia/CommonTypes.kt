package snakesoft.minion.materia

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json

@Serializable
data class Connection(
        var id: String,
        var A: String,
        var B: String,
        var type: String
)