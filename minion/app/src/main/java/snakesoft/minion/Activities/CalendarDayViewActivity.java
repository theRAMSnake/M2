package snakesoft.minion.Activities;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.List;
import java.util.Vector;

import calendar.Calendar;
import snakesoft.minion.Models.GlobalModel;
import snakesoft.minion.R;

public class CalendarDayViewActivity extends AppCompatActivity implements AdapterView.OnItemClickListener
{
    private List<Calendar.CalendarItem> mItems;
    private long mTimeStamp;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calendar_day_view);

        long timestamp = getIntent().getLongExtra("TimestampOnMidnight", 0);

        Vector<String> itemsAsString = new Vector<>();
        mTimeStamp = timestamp / 1000;
        long to = getNextDayInMillis(timestamp) / 1000;

        itemsAsString.add("New...");

        mItems = GlobalModel.getCalendarModel().getItems(mTimeStamp, to);
        for(Calendar.CalendarItem item : mItems)
        {
            java.util.Calendar calendar = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"));
            calendar.setTimeInMillis(item.getTimestamp() * 1000);

            String minutes = Integer.toString(calendar.get(java.util.Calendar.MINUTE));
            if(minutes.length() == 1)
            {
                minutes = "0" + minutes;
            }
            itemsAsString.add(
                    Integer.toString(calendar.get(java.util.Calendar.HOUR_OF_DAY)) +
                    ":" +
                    minutes +
                    " - " +
                    item.getText());
        }

        ArrayAdapter<String> itemsAdapter =
                new ArrayAdapter<>(this,
                        android.R.layout.simple_list_item_1,
                        itemsAsString);

        ListView listView = (ListView) findViewById(R.id.lvCalendarDay);
        listView.setAdapter(itemsAdapter);
        listView.setOnItemClickListener(this);
    }

    private long getNextDayInMillis(long millis) {
        // Add one day's time to the beginning of the day.
        // 24 hours * 60 minutes * 60 seconds * 1000 milliseconds = 1 day
        return millis + (24 * 60 * 60 * 1000);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == 1)
        {
            recreate();
        }
    }

    public void onItemClick(AdapterView<?> parent, View view, int position, long id)
    {
        if(position != 0)
        {
            Intent myIntent = new Intent(CalendarDayViewActivity.this, CalendarItemViewActivity.class);
            myIntent.putExtra("Text", mItems.get(position - 1).getText());
            myIntent.putExtra("Timestamp", mItems.get(position - 1).getTimestamp());
            myIntent.putExtra("IsNewItem", false);
            myIntent.putExtra("Id", mItems.get(position - 1).getId().getGuid());
            startActivityForResult(myIntent, 1);
        }
        else
        {
            Intent myIntent = new Intent(CalendarDayViewActivity.this, CalendarItemViewActivity.class);
            myIntent.putExtra("IsNewItem", true);
            myIntent.putExtra("Timestamp", mTimeStamp);
            myIntent.putExtra("Id", GlobalModel.getCalendarModel().getNewId());
            startActivityForResult(myIntent, 1);
        }
    }
}
