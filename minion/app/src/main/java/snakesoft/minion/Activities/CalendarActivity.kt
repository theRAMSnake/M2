package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.v4.view.PagerAdapter
import android.support.v4.view.ViewPager
import android.support.v7.app.AppCompatActivity
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import org.jetbrains.anko.*
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
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

                val m = textView(SimpleDateFormat("yyyy").format(Pages[position])) {
                    textSize = 24f
                    gravity = Gravity.CENTER
                    id = 999
                }.lparams(width = matchParent, height = wrapContent)

                textView(SimpleDateFormat("MMM").format(Pages[position])) {
                    textSize = 64f
                    gravity = Gravity.CENTER

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