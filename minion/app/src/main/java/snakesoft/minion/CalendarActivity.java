package snakesoft.minion;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.applandeo.materialcalendarview.CalendarView;
import com.applandeo.materialcalendarview.EventDay;
import com.applandeo.materialcalendarview.listeners.OnDayClickListener;
import com.applandeo.materialcalendarview.utils.DateUtils;

import java.util.ArrayList;
import java.util.List;

import calendar.Calendar;

public class CalendarActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calendar);

        List<EventDay> mEventDays = new ArrayList<>();
        CalendarView mCalendarView = (CalendarView) findViewById(R.id.cal_cal_view);

        for(Calendar.CalendarItem item : GlobalModel.getCalendarModel().getAllItems())
        {
            java.util.Calendar cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"));
            cal.setTimeInMillis(item.getTimestamp() * 1000);

            //Here we must recreate calendar item with local timezone, preserving the date only
            //Because of a bug CalendarView which allow to work with local time zones only
            java.util.Calendar calForEventDay = java.util.Calendar.getInstance();
            calForEventDay.set(
                    cal.get(java.util.Calendar.YEAR),
                    cal.get(java.util.Calendar.MONTH),
                    cal.get(java.util.Calendar.DAY_OF_MONTH));

            mEventDays.add(new EventDay(
                    calForEventDay,
                    getResources().getIdentifier("@mipmap/event", "mipmap", getPackageName()) ));
        }

        mCalendarView.setEvents(mEventDays);
        mCalendarView.setOnDayClickListener(new OnDayClickListener()
        {
            @Override
            public void onDayClick(EventDay eventDay)
            {
                if(eventDay != null)
                {
                    Intent myIntent = new Intent(CalendarActivity.this, CalendarDayViewActivity.class);

                    //Here we must recreate calendar item with UTC timezone, preserving the date only
                    //Because of a bug CalendarView which allow to work with local time zones only
                    java.util.Calendar calForMateria = java.util.Calendar.getInstance(
                            java.util.TimeZone.getTimeZone("UTC")
                    );
                    calForMateria.set(
                            eventDay.getCalendar().get(java.util.Calendar.YEAR),
                            eventDay.getCalendar().get(java.util.Calendar.MONTH),
                            eventDay.getCalendar().get(java.util.Calendar.DAY_OF_MONTH));
                    DateUtils.setMidnight(calForMateria);

                    myIntent.putExtra("TimestampOnMidnight", calForMateria.getTimeInMillis());
                    startActivityForResult(myIntent, 1);
                }
            }
        });
    }
}
