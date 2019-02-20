package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import org.jetbrains.anko.*
import org.jetbrains.anko.design.floatingActionButton
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.sdk27.coroutines.onItemClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.InboxItem

class InboxActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        InboxActivityUI().setContentView(this)
    }
}

class InboxAdapter : BaseAdapter()
{
    val Items = GlobalModel.InboxModel.Items

    override fun getView(i : Int, v : View?, parent : ViewGroup?) : View
    {
        val item = getItem(i)
        return with(parent!!.context) {
            relativeLayout {
                textView(item) {
                    textSize = 32f
                }
            }
        }
    }

    override fun getItem(position : Int) : String
    {
        return Items[position].text
    }

    override fun getCount() : Int
    {
        return Items.size
    }

    override fun getItemId(pos: Int): Long
    {
        return pos.toLong()
    }
}

class InboxActivityUI : MateriaActivityUI<InboxActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            listView()
            {
                adapter = InboxAdapter()

                onItemClick { _, _, pos: Int, _ ->
                    showInboxItemActivity(GlobalModel.InboxModel.Items[pos], ctx)
                }
            }

            floatingActionButton()
            {
                imageResource = android.R.drawable.ic_input_add
                onClick()
                {
                    showInboxItemActivity(GlobalModel.InboxModel.createItemTemplate(), ctx)
                }
            }
        }
    }

    private fun showInboxItemActivity(inboxItem: InboxItem, ctx: Context)
    {
        //ctx.startActivity<InboxItemActivity>("item" to inboxItem)
    }
}