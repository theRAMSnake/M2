package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import org.w3c.dom.Text
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.InboxItem
import java.util.*

fun getInvalidId() : java.util.UUID
{
    return java.util.UUID(0, 0)
}

class InboxActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        InboxActivityUI().setContentView(this)
    }
}

class InboxItemViewHandler: CollectionUIViewHandler()
{
    private var TextView: TextView? = null
    private var Id: UUID = getInvalidId()

    override fun createView(parent: ViewGroup): View
    {
        return with(parent.context) {
            relativeLayout {
                TextView = textView {
                    textSize = 32f
                    onClick { OnClicked(Id) }
                }
            }
        }
    }

    override fun bindToItem(id: UUID)
    {
        Id = id
        TextView?.text = GlobalModel.InboxModel.Items.byId(id).text
    }
}

class InboxCollectionUIProvider(private val ctx: Context): ICollectionUIProvider()
{
    override fun createItemViewHandler(): CollectionUIViewHandler
    {
        return InboxItemViewHandler()
    }

    override fun showEditDialog(id: UUID)
    {
        val item = GlobalModel.InboxModel.Items.byId(id)

        with(ctx)
        {
            alert {
                customView {
                    val t = editText(item.text)
                    okButton()
                    {
                        item.text = t.text.toString()
                        GlobalModel.InboxModel.Items.replace(item)
                        OnChanged()
                    }
                }
            }.show()
        }
    }

    override fun showAddDialog()
    {
        val item = InboxItem(getInvalidId(), "")

        with(ctx)
        {
            alert {
                customView {
                    val t = editText(item.text)
                    okButton()
                    {
                        item.text = t.text.toString()
                        GlobalModel.InboxModel.Items.add(item)
                        OnChanged()
                    }
                }
            }.show()
        }
    }

    override fun getItems(): List<UUID>
    {
        return GlobalModel.InboxModel.Items.getAvailableItems().map {it.id }.toList()
    }

    override fun deleteItem(id: UUID)
    {
        GlobalModel.InboxModel.Items.delete(id)
        OnChanged()
    }

    override fun restoreItem(id: UUID)
    {
        GlobalModel.InboxModel.Items.restore(id)
        OnChanged()
    }
}

class InboxActivityUI : MateriaActivityUI<InboxActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            relativeLayout()
            {
                fillCollectionViewUI(this, ctx, InboxCollectionUIProvider(ctx))
            }
        }
    }
}