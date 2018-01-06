package snakesoft.minion;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class SettingsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        final TextView txtView = (TextView) findViewById(R.id.edtIP);
        txtView.setText(GlobalModel.getIp());

        ((Button) findViewById(R.id.btnSave)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ip = txtView.getText().toString();
                GlobalModel.setNewIp(ip);

                Intent myIntent = new Intent(SettingsActivity.this, SyncActivity.class);
                SettingsActivity.this.startActivity(myIntent);
            }
        });

        ((Button) findViewById(R.id.set_btnReset)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(SettingsActivity.this);
                dlgAlert.setMessage("Are you sure?");
                dlgAlert.setTitle("Caution");
                dlgAlert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        GlobalModel.reset();

                        Intent myIntent = new Intent(SettingsActivity.this, SyncActivity.class);
                        SettingsActivity.this.startActivity(myIntent);
                    }});

                dlgAlert.setCancelable(true);
                dlgAlert.create().show();
            }
        });
    }
}
