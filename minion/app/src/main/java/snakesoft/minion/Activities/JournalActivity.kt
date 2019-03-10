package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import com.unnamed.b.atv.model.TreeNode
import org.jetbrains.anko.support.v4._DrawerLayout
import com.unnamed.b.atv.view.AndroidTreeView
import org.jetbrains.anko.*
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.JournalIndexItem


class JournalActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        JournalActivityUI().setContentView(this)
    }
}

class MyHolder(private val ctx: Context, val depth: Int): TreeNode.BaseNodeViewHolder<JournalIndexItem>(ctx)
{
    override fun createNodeView(node: TreeNode?, value: JournalIndexItem?): View
    {
        return with(ctx) {
            relativeLayout {
                textView(value!!.title) {
                    textSize = 32f
                    textColor = if(value!!.isPage) Color.BLUE else Color.GREEN
                }.lparams()
                {
                    leftMargin = dip(20 * depth)
                }
            }
        }
    }
}

class JournalActivityUI : MateriaActivityUI<JournalActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            relativeLayout()
            {
                val root = TreeNode.root()

                for (x in GlobalModel.JournalModel.Items.filter { it.folderId == getInvalidId() })
                {
                    fillNode(x, root, ctx, 0)
                }

                val tView = AndroidTreeView(ctx, root)
                addView(tView.view)
            }
        }
    }

    private fun fillNode(x: JournalIndexItem, parent: TreeNode, ctx: Context, depth: Int)
    {
        val newNode = TreeNode(x)
        newNode.viewHolder = MyHolder(ctx, depth)
        parent.addChild(newNode)

        for (y in GlobalModel.JournalModel.Items.filter { it.folderId == x.id })
        {
            fillNode(y, newNode, ctx, depth + 1)
        }
    }
}