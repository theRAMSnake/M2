package snakesoft.minion.Models

import com.google.common.collect.ImmutableCollection
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
        get() = getAvailableItems().size

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
        val item = Items[id]!!
        if(item.trackingInfo != StatusOfChange.Add)
        {
            item.trackingInfo = StatusOfChange.Delete
        }
        else
        {
            item.trackingInfo = StatusOfChange.Junk
        }

        Items[id] = item
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
        return getAvailableItems().elementAt(i)
    }

    fun getAvailableItems(): List<T>
    {
        return Items.values.filter { it.trackingInfo != StatusOfChange.Junk &&
                it.trackingInfo != StatusOfChange.Delete }
    }

    fun restore(id: UUID)
    {
        val item = Items[id]!!
        if(item.trackingInfo == StatusOfChange.Junk)
        {
            item.trackingInfo = StatusOfChange.Add
        }
        else
        {
            item.trackingInfo = StatusOfChange.None
        }

        Items[id] = item
        OnChanged()
    }

    fun byId(id: UUID): T
    {
        return Items[id]!!
    }
}