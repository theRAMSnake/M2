package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import org.w3c.dom.Text
import snakesoft.minion.Models.FinanceEvent
import snakesoft.minion.Models.GlobalModel
import java.util.*

class FinanceActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        FinanceActivityUI().setContentView(this)
    }
}

class FinanceItemViewHandler: CollectionUIViewHandler()
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
        val item = GlobalModel.FinanceModel.Events.first {it.eventId == id}
        val amount = item.amountCents / 100.0
        val text = item.details + ": " + String.format("%.2f", amount)
        TextView?.text = text
    }
}

class FinanceCollectionUIProvider(private val ctx: Context): ICollectionUIProvider()
{
    override fun createItemViewHandler(): CollectionUIViewHandler
    {
        return FinanceItemViewHandler()
    }

    override fun showEditDialog(id: UUID)
    {
        //Currently edit is not supported
    }

    override fun showAddDialog()
    {
        val item = FinanceEvent(getInvalidId(), getInvalidId(), "", 0, 0)

        with(ctx)
        {
            alert {
                customView {
                    var selected = getInvalidId()
                    radioGroup {
                        var i = 0
                        GlobalModel.FinanceModel.Categories.forEach {
                            val currentCat = it
                            radioButton {
                                id = i
                                text = currentCat.name

                                onClick {
                                    selected = currentCat.id
                                }
                            }
                            i++
                        }
                    }
                    okButton()
                    {
                        item.categoryId = selected

                        alert {
                            customView {
                                val details = editText(item.details)
                                val amount = editText(item.amountCents.toString())

                                okButton()
                                {
                                    item.categoryId = getInvalidId()
                                    item.details = details.text.toString()
                                    item.amountCents = (amount.text.toString().toDouble() * 100).toLong()
                                    GlobalModel.FinanceModel.addEvent(item)
                                    OnChanged()
                                }
                            }
                        }.show()
                    }
                }
            }.show()
        }
    }

    override fun getItems(): List<UUID>
    {
        return GlobalModel.FinanceModel.Events.map { it.eventId }.toList()
    }

    override fun deleteItem(id: UUID)
    {
        GlobalModel.FinanceModel.deleteEvent(id)
        OnChanged()
    }

    override fun restoreItem(id: UUID)
    {
        GlobalModel.FinanceModel.restoreLastEvent()
        OnChanged()
    }
}

class FinanceActivityUI : MateriaActivityUI<FinanceActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            relativeLayout()
            {
                fillCollectionViewUI(this, ctx, FinanceCollectionUIProvider(ctx))
            }
        }
    }
}