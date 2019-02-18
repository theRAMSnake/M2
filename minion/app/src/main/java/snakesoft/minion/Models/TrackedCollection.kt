package snakesoft.minion.Models

import java.util.*

interface ITrackable
{
    var id: java.util.UUID
    var trackingInfo: StatusOfChange
}

class TrackedCollection<T: ITrackable> : Iterable<T>
{
    private var Items: MutableMap<java.util.UUID, T> = mutableMapOf()

    override operator fun iterator(): Iterator<T> = Items.values.iterator()

    val size: Int
        get() = Items.size

    val OnChanged = Event()

    constructor(input: List<T>)
    {
        Items = input.associate{ it.id to it }.toMutableMap()
    }

    constructor()

    fun replace(item: T)
    {
        var newItem = item

        if(newItem.trackingInfo != StatusOfChange.Add)
        {
            newItem.trackingInfo = StatusOfChange.Edit
        }

        Items[newItem.id] = newItem

        OnChanged()
    }

    fun delete(id: java.util.UUID)
    {
        Items[id]!!.trackingInfo = StatusOfChange.Delete

        OnChanged()
    }

    fun add(item: T)
    {
        var newItem = item
        newItem.id = UUID.randomUUID()
        newItem.trackingInfo = StatusOfChange.Add

        Items[newItem.id] = newItem

        OnChanged()
    }

    fun clear()
    {
        Items.clear()
        OnChanged()
    }

    operator fun get(i: Int): T
    {
        return Items.values.elementAt(i)
    }
}