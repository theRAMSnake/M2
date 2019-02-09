package snakesoft.minion.Activities

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle

import com.applandeo.materialcalendarview.CalendarView
import com.applandeo.materialcalendarview.EventDay
import com.applandeo.materialcalendarview.listeners.OnDayClickListener
import com.applandeo.materialcalendarview.utils.DateUtils

import java.util.ArrayList

import calendar.Calendar
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class CalendarActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_calendar)

        val mEventDays = ArrayList<EventDay>()
        val mCalendarView = findViewById(R.id.cal_cal_view) as CalendarView

        for (item in GlobalModel.calendarModel!!.AllItems) {
            val cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"))
            cal.timeInMillis = item.timestamp * 1000

            //Here we must recreate calendar item with local timezone, preserving the date only
            //Because of a bug CalendarView which allow to work with local time zones only
            val calForEventDay = java.util.Calendar.getInstance()
            calForEventDay.set(
                    cal.get(java.util.Calendar.YEAR),
                    cal.get(java.util.Calendar.MONTH),
                    cal.get(java.util.Calendar.DAY_OF_MONTH))

            mEventDays.add(EventDay(
                    calForEventDay,
                    resources.getIdentifier("@mipmap/event", "mipmap", packageName)))
        }

        mCalendarView.setEvents(mEventDays)
        mCalendarView.setOnDayClickListener { eventDay ->
            if (eventDay != null) {
                val myIntent = Intent(this@CalendarActivity, CalendarDayViewActivity::class.java)

                //Here we must recreate calendar item with UTC timezone, preserving the date only
                //Because of a bug CalendarView which allow to work with local time zones only
                val calForMateria = java.util.Calendar.getInstance(
                        java.util.TimeZone.getTimeZone("UTC")
                )
                calForMateria.set(
                        eventDay.calendar.get(java.util.Calendar.YEAR),
                        eventDay.calendar.get(java.util.Calendar.MONTH),
                        eventDay.calendar.get(java.util.Calendar.DAY_OF_MONTH))
                DateUtils.setMidnight(calForMateria)

                myIntent.putExtra("TimestampOnMidnight", calForMateria.timeInMillis)
                startActivityForResult(myIntent, 1)
            }
        }
    }
}
