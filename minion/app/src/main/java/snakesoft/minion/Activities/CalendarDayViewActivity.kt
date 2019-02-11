package snakesoft.minion.Activities

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.ListView
import java.util.Vector

import calendar.Calendar
import snakesoft.minion.Models.CalendarItem
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class CalendarDayViewActivity : AppCompatActivity(), AdapterView.OnItemClickListener {
    private var Items: List<CalendarItem>? = null
    private var TimeStamp: Long = 0

    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_calendar_day_view)

        val timestamp = intent.getLongExtra("TimestampOnMidnight", 0)

        val itemsAsString = Vector<String>()
        TimeStamp = timestamp / 1000
        val to = getNextDayInMillis(timestamp) / 1000

        itemsAsString.add("New...")

        Items = GlobalModel.CalendarModel.Items.filter { it.timestamp in TimeStamp..to }
        for (item in Items!!)
        {
            val calendar = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"))
            calendar.timeInMillis = item.timestamp * 1000

            var minutes = Integer.toString(calendar.get(java.util.Calendar.MINUTE))
            if (minutes.length == 1) {
                minutes = "0$minutes"
            }
            itemsAsString.add(
                    Integer.toString(calendar.get(java.util.Calendar.HOUR_OF_DAY)) +
                            ":" +
                            minutes +
                            " - " +
                            item.text)
        }

        val itemsAdapter = ArrayAdapter(this,
                android.R.layout.simple_list_item_1,
                itemsAsString)

        val listView = findViewById(R.id.lvCalendarDay) as ListView
        listView.adapter = itemsAdapter
        listView.onItemClickListener = this
    }

    private fun getNextDayInMillis(millis: Long): Long
    {
        // Add one day's time to the beginning of the day.
        // 24 hours * 60 minutes * 60 seconds * 1000 milliseconds = 1 day
        return millis + 24 * 60 * 60 * 1000
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent)
    {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == 1)
        {
            recreate()
        }
    }

    override fun onItemClick(parent: AdapterView<*>, view: View, position: Int, id: Long)
    {
        if (position != 0)
        {
            val myIntent = Intent(this@CalendarDayViewActivity, CalendarItemViewActivity::class.java)
            myIntent.putExtra("Text", Items!![position - 1].text)
            myIntent.putExtra("Timestamp", Items!![position - 1].timestamp)
            myIntent.putExtra("IsNewItem", false)
            myIntent.putExtra("Id", Items!![position - 1].id.toString())
            startActivityForResult(myIntent, 1)
        }
        else
        {
            val myIntent = Intent(this@CalendarDayViewActivity, CalendarItemViewActivity::class.java)
            myIntent.putExtra("IsNewItem", true)
            myIntent.putExtra("Timestamp", TimeStamp)
            myIntent.putExtra("Id", java.util.UUID.randomUUID().toString())
            startActivityForResult(myIntent, 1)
        }
    }
}
