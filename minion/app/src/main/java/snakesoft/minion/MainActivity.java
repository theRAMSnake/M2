package snakesoft.minion;

import android.content.ContentUris;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.provider.CalendarContract;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;



public class MainActivity extends AppCompatActivity implements View.OnClickListener
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mSyncBtn = (Button) findViewById(R.id.btnSync);
        mSyncBtn.setOnClickListener(MainActivity.this);

        mInboxBtn = (Button) findViewById(R.id.btnInbox);
        mInboxBtn.setOnClickListener(MainActivity.this);

        mActionsBtn = (Button) findViewById(R.id.btnActions);
        mActionsBtn.setOnClickListener(MainActivity.this);

        mSettingsBtn = (Button) findViewById(R.id.btnSettings);
        mSettingsBtn.setOnClickListener(MainActivity.this);

        mCalendarBtn = (Button) findViewById(R.id.btnCalendar);
        mCalendarBtn.setOnClickListener(MainActivity.this);

        mBacklogBtn = (Button) findViewById(R.id.btnBacklog);
        mBacklogBtn.setOnClickListener(MainActivity.this);

        GlobalModel.init(getApplicationContext());
    }

    @Override
    public void onClick(View v)
    {
        if(v == mInboxBtn)
        {
            Intent myIntent = new Intent(MainActivity.this, InboxActivity.class);
            MainActivity.this.startActivity(myIntent);
        }
        if(v == mSyncBtn)
        {
            Intent myIntent = new Intent(MainActivity.this, SyncActivity.class);
            MainActivity.this.startActivity(myIntent);
        }
        if(v == mActionsBtn)
        {
            Intent myIntent = new Intent(MainActivity.this, ActionsActivity.class);
            myIntent.putExtra("Mode", "Normal");
            MainActivity.this.startActivity(myIntent);
        }
        if(v == mSettingsBtn)
        {
            Intent myIntent = new Intent(MainActivity.this, SettingsActivity.class);
            MainActivity.this.startActivity(myIntent);
        }
        if(v == mCalendarBtn)
        {
            Intent myIntent = new Intent(MainActivity.this, CalendarActivity.class);
            startActivity(myIntent);
        }
        if(v == mBacklogBtn)
        {
            Intent myIntent = new Intent(MainActivity.this, ActionsActivity.class);
            myIntent.putExtra("Mode", "Backlog");
            MainActivity.this.startActivity(myIntent);
        }
    }

    private Button mSyncBtn;
    private Button mInboxBtn;
    private Button mActionsBtn;
    private Button mSettingsBtn;
    private Button mCalendarBtn;
    private Button mBacklogBtn;
}
