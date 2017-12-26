package snakesoft.minion;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import calendar.Calendar;
import common.Common;

public class CalendarItemViewActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calendar_item_view);

        TextView txtView = (TextView) findViewById(R.id.cal_txtText);
        txtView.setText(getIntent().getStringExtra("Text"));

        TextView txtViewTime = (TextView) findViewById(R.id.cal_txtTextTime);
        txtViewTime.setText(getTimeText(getIntent().getLongExtra("Timestamp", 0)));

        TextView txtViewDate = (TextView) findViewById(R.id.cal_txtTextDate);
        txtViewDate.setText(getDateText(getIntent().getLongExtra("Timestamp", 0)));

        mAcceptBtn = (Button) findViewById(R.id.cal_btnAccept);
        mAcceptBtn.setOnClickListener(CalendarItemViewActivity.this);

        mBtnDelete = (Button) findViewById(R.id.cal_btnDelete);
        mBtnDelete.setOnClickListener(CalendarItemViewActivity.this);

        if(getIntent().getBooleanExtra("IsNewItem", false))
        {
            mBtnDelete.setVisibility(View.INVISIBLE);
        }
    }

    private String getDateText(long timestamp)
    {
        java.util.Calendar cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"));
        cal.setTimeInMillis(timestamp * 1000);

        String days = Integer.toString(cal.get(java.util.Calendar.DAY_OF_MONTH));
        if(days.length() == 1)
        {
            days = "0" + days;
        }

        String month = Integer.toString(cal.get(java.util.Calendar.MONTH) + 1);
        if(month.length() == 1)
        {
            month = "0" + month;
        }

        return days + "/" + month + "/" + Integer.toString(cal.get(java.util.Calendar.YEAR));
    }

    private String getTimeText(long timestamp)
    {
        java.util.Calendar cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"));
        cal.setTimeInMillis(timestamp * 1000);

        String minutes = Integer.toString(cal.get(java.util.Calendar.MINUTE));
        if(minutes.length() == 1)
        {
            minutes = "0" + minutes;
        }

        return Integer.toString(cal.get(java.util.Calendar.HOUR_OF_DAY)) + ":" +
                minutes;
    }

    @Override
    public void onClick(View v)
    {
        final String guid = getIntent().getStringExtra("Id");
        if(v == mAcceptBtn)
        {
            TextView txtView = (TextView) findViewById(R.id.cal_txtText);
            TextView txtViewTime = (TextView) findViewById(R.id.cal_txtTextTime);
            TextView txtViewDate = (TextView) findViewById(R.id.cal_txtTextDate);

            long timestamp = parseTimeAndDateText(
                    txtViewTime.getText().toString(),
                    txtViewDate.getText().toString());

            if(timestamp == 0)
            {
                Context context = getApplicationContext();
                CharSequence text = "Error in parsing text/date fields";
                int duration = Toast.LENGTH_SHORT;

                Toast toast = Toast.makeText(context, text, duration);
                toast.show();
                return;
            }

            Calendar.CalendarItem result = Calendar.CalendarItem.newBuilder()
                    .setText(txtView.getText().toString())
                    .setTimestamp(timestamp)
                    .setId(Common.UniqueId.newBuilder().setGuid(guid).build())
                    .build();

            if(getIntent().getBooleanExtra("IsNewItem", false))
            {
                GlobalModel.getCalendarModel().addItem(result);
            }
            else
            {
                GlobalModel.getCalendarModel().modifyItem(result);
            }
            finish();
        }
        if(v == mBtnDelete)
        {
            AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(this);
            dlgAlert.setMessage("Are you sure?");
            dlgAlert.setTitle("Caution");
            dlgAlert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    GlobalModel.getCalendarModel().deleteItem(guid);
                    finish();
                }});

            dlgAlert.setCancelable(true);
            dlgAlert.create().show();
        }
    }

    private long parseTimeAndDateText(String timeS, String dateS)
    {
        try
        {
            java.util.Calendar cal = java.util.Calendar.getInstance(java.util.TimeZone.getTimeZone("UTC"));
            String[] values = timeS.split(":");

            int hours = Integer.valueOf(values[0]);
            int minutes = Integer.valueOf(values[1]);

            values = dateS.split("/");

            int day = Integer.valueOf(values[0]);
            int month = Integer.valueOf(values[1]) - 1;
            int year = Integer.valueOf(values[2]);

            cal.set(year, month, day, hours, minutes);

            return cal.getTimeInMillis() / 1000;
        }
        catch (Exception ex)
        {
            return 0;
        }
    }

    private Button mBtnDelete;
    private Button mAcceptBtn;
}
