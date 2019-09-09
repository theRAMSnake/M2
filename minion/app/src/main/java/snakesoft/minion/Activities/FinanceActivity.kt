package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.net.Uri
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.util.Log
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

data class SMSData(
    var date: Long = 0,
    var address: String = "",
    var body: String = ""
)

class FinanceActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)

        readSMSs()

        FinanceActivityUI().setContentView(this)
    }

    private fun readSMSs()
    {
        val cursor = contentResolver.query(Uri.parse("content://sms/inbox"), null, null, null, null)

        if (cursor!!.moveToFirst()) { // must check the result to prevent exception
            do {
                var curData = SMSData()

                for (idx in 0 until cursor.columnCount)
                {
                    val text = cursor.getString(idx)
                    when {
                        cursor.getColumnName(idx) == "address" -> curData.address = text
                        cursor.getColumnName(idx) == "date" -> curData.date = text.toLong() / 1000
                        cursor.getColumnName(idx) == "body" -> curData.body = text
                    }
                }

                if(curData.date > GlobalModel.FinanceModel.LastSMSReadDate &&
                        curData.address == "BOC Message" &&
                        curData.body.startsWith("Your"))
                {
                    GlobalModel.FinanceModel.addPreEvent(curData.date, extractDetails(curData.body), extractAmount(curData.body))
                    GlobalModel.FinanceModel.updateLastSMSReadDate(curData.date)
                }

                // use msgData
            } while (cursor.moveToNext())
        } else {
            // empty box, no SMS
        }
    }

    private fun extractAmount(body: String): Long {
        val pos = body.indexOf("amount of ")

        return (body.substring(pos + 10).replace(',', '.').toDouble() * 100).toLong()
    }

    private fun extractDetails(body: String): String {
        val pos = body.indexOf("use at ")
        val endPos = body.indexOf(" on ")

        return body.substring(pos + 7, endPos)
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
        TextView?.textColor = if (item.categoryId == getInvalidId()) Color.RED else Color.BLUE
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
        //Edit only allows to assign category
        val item = GlobalModel.FinanceModel.Events.first {it.eventId == id}

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
                                this.id = i
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
                        GlobalModel.FinanceModel.updateEvent(item)
                    }
                }
            }.show()
        }
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

                                okButton()
                                {
                                    item.details = details.text.toString()
                                    alert {
                                        customView {
                                            val amount = editText(String.format("%.2f", item.amountCents / 100.0))

                                            okButton()
                                            {
                                                item.amountCents = (amount.text.toString().toDouble() * 100).toLong()
                                                item.timestamp = System.currentTimeMillis() / 1000
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