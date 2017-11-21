package snakesoft.minion;

import android.app.AlertDialog;
import android.content.DialogInterface;
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

    @Override
    public void onSyncError()
    {
        AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(this);
        dlgAlert.setMessage("Sync error: materia unreachable");
        dlgAlert.setTitle("Warning");
        dlgAlert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                finish();
            }});

        dlgAlert.create().show();
    }
}
