package snakesoft.minion.Models

class TrackedCollection<T>
{
    private var Items: MutableMap<java.util.UUID, T> = mutableMapOf()
}