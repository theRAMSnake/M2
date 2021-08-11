package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import androidx.viewpager.widget.PagerAdapter
import androidx.viewpager.widget.ViewPager
import androidx.appcompat.app.AppCompatActivity
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.materia.CalendarItem
import java.text.SimpleDateFormat
import java.time.format.DateTimeFormatter
import java.util.*

class CalendarActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)

        CalendarActivityUI().setContentView(this)
    }
}

class CalendarItemViewHandler: CollectionUIViewHandler()
{
    private var TextView: TextView? = null
    private var DayView: TextView? = null
    private var TimeView: TextView? = null
    private var Id: UUID = getInvalidId()

    override fun createView(parent: ViewGroup): View
    {
        return with(parent.context) {
            relativeLayout {
                DayView = textView {
                    textSize = 48f
                    id = 55
                }.lparams(width = wrapContent, height = matchParent)
                {
                    gravity = Gravity.TOP
                }
                TimeView = textView {
                    textSize = 12f
                    id = 56
                }.lparams()
                {
                    leftMargin = dip(10)
                    rightOf(55)
                }
                TextView = textView {
                    textSize = 24f
                    onClick { OnClicked(Id) }
                }.lparams()
                {
                    leftMargin = dip(10)
                    rightOf(55)
                    bottomOf(56)
                }
            }
        }
    }

    override fun bindToItem(id: UUID)
    {
        Id = id
        val item = GlobalModel.CalendarModel.Items.byId(id)
        TextView?.text = item.text

        val datetime = Date(item.timestamp * 1000)

        DayView?.text = SimpleDateFormat("dd", Locale.UK).format(datetime)

        val df = SimpleDateFormat("hh:mm", Locale.UK)
        df.timeZone = TimeZone.getTimeZone("UTC")
        TimeView?.text = df.format(datetime)

        val cal = Calendar.getInstance()
        cal.set(cal.get(Calendar.YEAR), cal.get(Calendar.MONTH), cal.get(Calendar.DAY_OF_MONTH), 0, 0, 0)
        cal.add(Calendar.DAY_OF_MONTH, 1)
        val nextDate = cal.time

        if(datetime < nextDate)
        {
            TextView?.textColor = Color.BLUE
            DayView?.textColor = Color.BLUE
            TimeView?.textColor = Color.BLUE
        }
        else
        {
            TextView?.textColor = Color.GRAY
            DayView?.textColor = Color.GRAY
            TimeView?.textColor = Color.GRAY
        }
    }
}

class CallendarCollectionUIProvider(private val ctx: Context, private val Date: Date): ICollectionUIProvider()
{
    override fun createItemViewHandler(): CollectionUIViewHandler
    {
        return CalendarItemViewHandler()
    }

    fun showDialog(item: CalendarItem, func: (item: CalendarItem) -> Unit)
    {
        val cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"))

        if(item.timestamp != 0L)
        {
            cal.timeInMillis = item.timestamp * 1000
        }
        else
        {
            cal.set(Calendar.HOUR, 9)
            cal.set(Calendar.MINUTE, 0)
        }

        with(ctx)
        {
            alert {
                customView {
                    val txt = editText(item.text)
                    okButton()
                    {
                        alert {
                            customView {
                                val d = datePicker()
                                {
                                    updateDate(cal.get(Calendar.YEAR), cal.get(Calendar.MONTH), cal.get(Calendar.DAY_OF_MONTH))
                                }
                                okButton()
                                {
                                    alert {
                                        customView {
                                            val t = timePicker()
                                            {
                                                hour = cal.get(Calendar.HOUR)
                                                minute = cal.get(Calendar.MINUTE)
                                            }
                                            okButton()
                                            {
                                                item.text = txt.text.toString()
                                                item.timestamp = composeTimestamp(d.year, d.month, d.dayOfMonth, t.hour, t.minute)
                                                func(item)
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

    override fun showEditDialog(id: UUID)
    {
        val item = GlobalModel.CalendarModel.Items.byId(id)

        showDialog(item)
        {
            x -> GlobalModel.CalendarModel.Items.replace(x)
        }
    }

    override fun showAddDialog()
    {
        val item = CalendarItem(getInvalidId(), "", 0)

        showDialog(item)
        {
            x -> GlobalModel.CalendarModel.Items.add(x)
        }
    }

    private fun composeTimestamp(year: Int, month: Int, dayOfMonth: Int, hour: Int, minute: Int): Long
    {
        val cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"))
        cal.set(year, month, dayOfMonth, hour, minute, 0)
        return cal.timeInMillis / 1000
    }

    override fun getItems(): List<UUID>
    {
        return fetchMonthItemIds(Date)
    }

    private fun fetchMonthItemIds(date: Date): List<UUID>
    {
        val from = date

        val cal = Calendar.getInstance()
        cal.time = date
        cal.add(Calendar.MONTH, 1)

        val to = cal.time

        return GlobalModel.CalendarModel.Items.getAvailableItems().filter()
        {
            val d = Date(it.timestamp * 1000)
            d >= from && d < to
        }.sortedBy { it.timestamp  }.map { it.id }
    }

    override fun deleteItem(id: UUID)
    {
        GlobalModel.CalendarModel.Items.delete(id)
        OnChanged()
    }

    override fun restoreItem(id: UUID)
    {
        GlobalModel.CalendarModel.Items.restore(id)
        OnChanged()
    }
}

class MonthesAdapter: PagerAdapter()
{
    val Pages = mutableListOf<Date>()
    val TOTAL_PAGES = 12 //One year

    init
    {
        val availableItems = GlobalModel.CalendarModel.Items.getAvailableItems()
        if(availableItems.isNotEmpty())
        {
            val minDate = Date(availableItems.sortedBy { it.timestamp  }[0].timestamp * 1000)
            val cal = Calendar.getInstance()
            cal.time = minDate
            cal.set(Calendar.DAY_OF_MONTH, 1)
            cal.set(Calendar.HOUR_OF_DAY, 0)
            cal.set(Calendar.MINUTE, 0)

            for (i in 0..TOTAL_PAGES)
            {
                Pages.add(cal.time)

                cal.add(Calendar.MONTH, 1)
            }
        }
    }

    override fun getCount(): Int
    {
        return TOTAL_PAGES
    }

    override fun isViewFromObject(view: View, `object`: Any): Boolean
    {
        return view == `object`
    }

    override fun instantiateItem(container: ViewGroup, position: Int): Any
    {
        val result = with(container.context) {
            relativeLayout {
                lparams(width = matchParent, height = matchParent)

                val y = textView(SimpleDateFormat("yyyy").format(Pages[position])) {
                    textSize = 24f
                    gravity = Gravity.CENTER
                    id = 999
                }.lparams(width = matchParent, height = wrapContent)

                val m = textView(SimpleDateFormat("MMM").format(Pages[position])) {
                    textSize = 64f
                    gravity = Gravity.CENTER
                    id = 1000
                }.lparams(width = matchParent, height = wrapContent)
                {
                    below(y)
                }

                relativeLayout()
                {
                    fillCollectionViewUI(this, container.context, CallendarCollectionUIProvider(container.context, Pages[position]))
                }.lparams(width = matchParent, height = wrapContent)
                {
                    below(m)
                }
            }
        }

        container.addView(result)

        return result
    }

    override fun destroyItem(container: ViewGroup, position: Int, `object`: Any)
    {
        container.removeView(`object` as View)
    }
}

class CalendarActivityUI : MateriaActivityUI<CalendarActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            val p = ViewPager(ctx)
            p.adapter = MonthesAdapter()
            addView(p)
        }
    }
}