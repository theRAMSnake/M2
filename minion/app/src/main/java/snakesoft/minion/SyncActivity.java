package snakesoft.minion;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class SyncActivity extends AppCompatActivity implements SyncListener
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sync);

        GlobalModel.sync(this);
    }

    public void onSyncComplete()
    {
        finish();
    }
}
