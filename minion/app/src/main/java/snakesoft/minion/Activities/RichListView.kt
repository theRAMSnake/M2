package snakesoft.minion.Activities

import android.support.v7.widget.RecyclerView
import android.support.v7.widget.helper.ItemTouchHelper
import android.view.View
import android.view.ViewGroup
import snakesoft.minion.Models.Event1
import snakesoft.minion.Models.GlobalModel
import java.util.*

class SwipeToDeleteCallback(private val Adapter: RichListViewAdapter)
    : ItemTouchHelper.SimpleCallback(0, ItemTouchHelper.LEFT or ItemTouchHelper.RIGHT)
{
    override fun onMove(recyclerView: RecyclerView?, viewHolder: RecyclerView.ViewHolder?, target: RecyclerView.ViewHolder?): Boolean
    {
        return true
    }

    override fun onSwiped(viewHolder: RecyclerView.ViewHolder?, direction: Int)
    {
        val position = viewHolder!!.adapterPosition
        Adapter.deleteItem(position)
    }
}

class RichListViewAdapter(private val Provider: ICollectionUIProvider): RecyclerView.Adapter<RichListViewAdapter.ViewHolder>()
{
    val OnItemClicked = Event1<UUID>()
    val OnItemDeleted = Event1<java.util.UUID>()
    private var LastDeletedItemId = getInvalidId()
    private var Items = listOf<UUID>()

    init
    {
        Items = Provider.getItems()
        Provider.OnChanged += {
            Items = Provider.getItems()
            notifyDataSetChanged()
        }
    }

    class ViewHolder(val View: View, val Handler: CollectionUIViewHandler) : RecyclerView.ViewHolder(View)

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RichListViewAdapter.ViewHolder
    {
        val h = Provider.createItemViewHandler()
        val holder = ViewHolder(h.createView(parent), h)
        h.OnClicked += { id -> OnItemClicked(id) }
        return holder
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int)
    {
        holder.Handler.bindToItem(Items[position])
    }

    override fun getItemCount() = Items.size

    fun deleteItem(position: Int)
    {
        val id = Items[position]
        Provider.deleteItem(id)
        LastDeletedItemId = id
        OnItemDeleted(id)
    }

    fun undoDelete()
    {
        Provider.restoreItem(LastDeletedItemId)
    }
}