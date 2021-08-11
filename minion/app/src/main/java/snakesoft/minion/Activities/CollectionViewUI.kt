package snakesoft.minion.Activities

import android.content.Context
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.ItemTouchHelper
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.RelativeLayout
import org.jetbrains.anko.*
import org.jetbrains.anko.design.floatingActionButton
import org.jetbrains.anko.design.longSnackbar
import org.jetbrains.anko.sdk27.coroutines.onClick
import snakesoft.minion.Models.Event
import snakesoft.minion.Models.Event1
import java.util.*

abstract class CollectionUIViewHandler
{
    val OnClicked = Event1<UUID>()

    abstract fun bindToItem(id: UUID)
    abstract fun createView(parent: ViewGroup): View
}

abstract class ICollectionUIProvider
{
    val OnChanged = Event()

    abstract fun createItemViewHandler(): CollectionUIViewHandler
    abstract fun showEditDialog(id: UUID)
    abstract fun showAddDialog()

    abstract fun getItems(): List<UUID>
    abstract fun deleteItem(id: UUID)
    abstract fun restoreItem(id: UUID)
}

fun fillCollectionViewUI(l: @AnkoViewDslMarker _RelativeLayout, ctx: Context, provider: ICollectionUIProvider)
{
    with(l)
    {
        addView(RecyclerView(ctx).apply()
        {
            val ad = RichListViewAdapter(provider)
            adapter = ad
            layoutManager = LinearLayoutManager(ctx)
            ad.OnItemClicked += { id -> provider.showEditDialog(id) }
            ad.OnItemDeleted += {
                l.longSnackbar("Undo delete", "Undo") { ad.undoDelete() }
            }
            val itemTouchHelper = ItemTouchHelper(SwipeToDeleteCallback(ad))
            itemTouchHelper.attachToRecyclerView(this)
        })

        floatingActionButton()
        {
            imageResource = android.R.drawable.ic_input_add
            onClick()
            {
                provider.showAddDialog()
            }
        }.lparams()
        {
            margin = dip(10)
            alignParentBottom()
            alignParentEnd()
            alignParentRight()
            gravity = Gravity.BOTTOM or Gravity.END
        }
    }
}