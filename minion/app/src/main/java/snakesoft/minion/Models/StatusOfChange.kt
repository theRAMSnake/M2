package snakesoft.minion.Models

class StatusOfChange : java.io.Serializable {

    var type: Type

    enum class Type {
        None,
        Add,
        Delete,
        Edit,
        Junk
    }

    init {
        type = Type.None
    }
}
