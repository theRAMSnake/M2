package snakesoft.minion.Activities

import android.app.AlertDialog
import android.content.Context
import android.content.DialogInterface
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.TextView
import android.widget.Toast

import calendar.Calendar
import common.Common
import snakesoft.minion.Models.CalendarItem
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class CalendarItemViewActivity : AppCompatActivity(), View.OnClickListener {

    private var mBtnDelete: Button? = null
    private var mAcceptBtn: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_calendar_item_view)

        val txtView = findViewById(R.id.cal_txtText) as TextView
        txtView.text = intent.getStringExtra("Text")

        val txtViewTime = findViewById(R.id.cal_txtTextTime) as TextView
        txtViewTime.text = getTimeText(intent.getLongExtra("Timestamp", 0))

        val txtViewDate = findViewById(R.id.cal_txtTextDate) as TextView
        txtViewDate.text = getDateText(intent.getLongExtra("Timestamp", 0))

        mAcceptBtn = findViewById(R.id.cal_btnAccept) as Button
        mAcceptBtn!!.setOnClickListener(this@CalendarItemViewActivity)

        mBtnDelete = findViewById(R.id.cal_btnDelete) as Button
        mBtnDelete!!.setOnClickListener(this@CalendarItemViewActivity)

        if (intent.getBooleanExtra("IsNewItem", false)) {
            mBtnDelete!!.visibility = View.INVISIBLE
        }
    }

    private fun getDateText(timestamp: Long): String {
        val cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"))
        cal.timeInMillis = timestamp * 1000

        var days = Integer.toString(cal.get(java.util.Calendar.DAY_OF_MONTH))
        if (days.length == 1) {
            days = "0$days"
        }

        var month = Integer.toString(cal.get(java.util.Calendar.MONTH) + 1)
        if (month.length == 1) {
            month = "0$month"
        }

        return days + "/" + month + "/" + Integer.toString(cal.get(java.util.Calendar.YEAR))
    }

    private fun getTimeText(timestamp: Long): String {
        val cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"))
        cal.timeInMillis = timestamp * 1000

        var minutes = Integer.toString(cal.get(java.util.Calendar.MINUTE))
        if (minutes.length == 1) {
            minutes = "0$minutes"
        }

        return Integer.toString(cal.get(java.util.Calendar.HOUR_OF_DAY)) + ":" +
                minutes
    }

    override fun onClick(v: View) {
        val guid = java.util.UUID.fromString(intent.getStringExtra("Id"))
        if (v === mAcceptBtn) {
            val txtView = findViewById(R.id.cal_txtText) as TextView
            val txtViewTime = findViewById(R.id.cal_txtTextTime) as TextView
            val txtViewDate = findViewById(R.id.cal_txtTextDate) as TextView

            val timestamp = parseTimeAndDateText(
                    txtViewTime.text.toString(),
                    txtViewDate.text.toString())

            if (timestamp == 0L) {
                val context = applicationContext
                val text = "Error in parsing text/date fields"
                val duration = Toast.LENGTH_SHORT

                val toast = Toast.makeText(context, text, duration)
                toast.show()
                return
            }

            val result = CalendarItem(guid, txtView.text.toString(), timestamp);

            if (intent.getBooleanExtra("IsNewItem", false)) {
                GlobalModel.calendarModel!!.addItem(result)
            } else {
                GlobalModel.calendarModel!!.replaceItem(result)
            }
            finish()
        }
        if (v === mBtnDelete) {
            val dlgAlert = AlertDialog.Builder(this)
            dlgAlert.setMessage("Are you sure?")
            dlgAlert.setTitle("Caution")
            dlgAlert.setPositiveButton("OK") { dialog, which ->
                GlobalModel.calendarModel!!.deleteItem(guid)
                finish()
            }

            dlgAlert.setCancelable(true)
            dlgAlert.create().show()
        }
    }

    private fun parseTimeAndDateText(timeS: String, dateS: String): Long {
        try {
            val cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"))
            var values = timeS.split(":".toRegex()).dropLastWhile({ it.isEmpty() }).toTypedArray()

            val hours = Integer.valueOf(values[0])
            val minutes = Integer.valueOf(values[1])

            values = dateS.split("/".toRegex()).dropLastWhile({ it.isEmpty() }).toTypedArray()

            val day = Integer.valueOf(values[0])
            val month = Integer.valueOf(values[1]) - 1
            val year = Integer.valueOf(values[2])

            cal.set(year, month, day, hours, minutes)

            return cal.timeInMillis / 1000
        } catch (ex: Exception) {
            return 0
        }

    }
}
