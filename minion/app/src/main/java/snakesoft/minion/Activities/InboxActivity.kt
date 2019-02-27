package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import android.provider.Settings
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.support.v7.widget.helper.ItemTouchHelper
import android.widget.BaseAdapter
import org.jetbrains.anko.*
import org.jetbrains.anko.design.floatingActionButton
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.sdk27.coroutines.onItemClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.InboxItem
import android.text.method.Touch.onTouchEvent
import android.view.*
import android.widget.TextView
import snakesoft.minion.Models.Event1
import android.support.v4.view.accessibility.AccessibilityEventCompat.setAction
import android.support.design.widget.Snackbar
import org.jetbrains.anko.design.longSnackbar
import org.jetbrains.anko.design.snackbar


class InboxActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        InboxActivityUI().setContentView(this)
    }
}

class SwipeToDeleteCallback(private val Adapter: InboxAdapter)
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

class InboxAdapter: RecyclerView.Adapter<InboxAdapter.ViewHolder>()
{
    val OnItemClicked = Event1<InboxItem>()
    val OnItemDeleted = Event1<java.util.UUID>()
    var LastDeletedItemId = getInvalidId()

    class ViewHolder(view: View, private val tv: TextView) : RecyclerView.ViewHolder(view)
    {
        val OnItemClicked = Event1<InboxItem>()

        var Item: InboxItem = InboxItem(getInvalidId(), "")
            set(value)
            {
                tv.text = value.text
                field = value
            }

        init
        {
            tv.onClick { OnItemClicked(Item) }
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): InboxAdapter.ViewHolder
    {
        var t: TextView? = null
        val v = with(parent.context) {
            relativeLayout {
                t = textView {
                    textSize = 32f
                }
            }
        }

        val holder = ViewHolder(v, t!!)
        holder.OnItemClicked += { item -> OnItemClicked(item) }
        return holder
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int)
    {
        holder.Item = GlobalModel.InboxModel.Items[position]
    }

    override fun getItemCount() = GlobalModel.InboxModel.Items.size

    fun deleteItem(position: Int)
    {
        val id = GlobalModel.InboxModel.Items[position].id
        GlobalModel.InboxModel.Items.delete(id)
        notifyItemRemoved(position)
        LastDeletedItemId = id
        OnItemDeleted(id)
    }

    fun undoDelete()
    {
        GlobalModel.InboxModel.Items.restore(LastDeletedItemId)
        notifyDataSetChanged()
    }
}

fun getInvalidId() : java.util.UUID
{
    return java.util.UUID(0, 0)
}

class InboxActivityUI : MateriaActivityUI<InboxActivity>()
{
    private val Adapter = InboxAdapter()

    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            relativeLayout()
            {
                addView(RecyclerView(ctx).apply()
                {
                    adapter = Adapter
                    layoutManager = LinearLayoutManager(ctx)
                    Adapter.OnItemClicked += { item -> showInboxItemActivity(item, ctx) }
                    Adapter.OnItemDeleted += {
                        _DrawerLayout.longSnackbar("Undo delete", "Undo") { Adapter.undoDelete() }
                    }
                    val itemTouchHelper = ItemTouchHelper(SwipeToDeleteCallback(Adapter))
                    itemTouchHelper.attachToRecyclerView(this)
                })

                floatingActionButton()
                {
                    imageResource = android.R.drawable.ic_input_add
                    onClick()
                    {
                        showInboxItemActivity(InboxItem(getInvalidId(), ""), ctx)
                    }
                }.lparams {
                    //setting button to bottom right of the screen
                    margin = dip(10)
                    alignParentBottom()
                    alignParentEnd()
                    alignParentRight()
                    gravity = Gravity.BOTTOM or Gravity.END
                }
            }
        }
    }

    private fun showInboxItemActivity(inboxItem: InboxItem, ctx: Context)
    {
        with(ctx)
        {
            alert {
                customView {
                    val t = editText(inboxItem.text)
                    okButton()
                    {
                        inboxItem.text = t.text.toString()
                        if(inboxItem.id == getInvalidId())
                        {
                            GlobalModel.InboxModel.Items.add(inboxItem)
                        }
                        else
                        {
                            GlobalModel.InboxModel.Items.replace(inboxItem)
                        }
                        Adapter.notifyDataSetChanged()
                    }
                }
            }.show()
        }
    }
}