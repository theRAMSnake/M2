package snakesoft.minion.Activities;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import snakesoft.minion.Models.GlobalModel;
import snakesoft.minion.Models.SyncObserver;
import snakesoft.minion.R;

public class SyncActivity extends AppCompatActivity implements SyncListener
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sync);

        final EditText txtView = (EditText) findViewById(R.id.syncText);
        txtView.setEnabled(false);
        GlobalModel.sync(this);
    }

    public void onSyncComplete()
    {
        SyncObserver obs = GlobalModel.getSyncObserver();

        final ProgressBar pb = (ProgressBar) findViewById(R.id.pbSync);
        pb.setVisibility(View.INVISIBLE);

        final EditText txtView = (EditText) findViewById(R.id.syncText);
        txtView.setText(obs.getLog());
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
