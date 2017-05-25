package snakesoft.minion;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import common.Common;
import inbox.Inbox;

public class InboxItemViewActivity extends AppCompatActivity implements View.OnClickListener{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_inbox_item_view);

        mAcceptBtn = (Button) findViewById(R.id.btnAccept);
        mAcceptBtn.setOnClickListener(InboxItemViewActivity.this);

        mBtnDelete = (Button) findViewById(R.id.btnDelete);
        mBtnDelete.setOnClickListener(InboxItemViewActivity.this);

        if(getIntent().getBooleanExtra("IsNewItem", false))
        {
            mBtnDelete.setVisibility(View.INVISIBLE);
            TextView txtView = (TextView) findViewById(R.id.txtText);
            txtView.requestFocus();
            InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY);
        }
        else
        {
            TextView txtView = (TextView) findViewById(R.id.txtText);
            txtView.setText(getIntent().getStringExtra("Text"));
        }
    }

    @Override
    public void onClick(View v)
    {
        final String guid = getIntent().getStringExtra("Id");
        if(v == mAcceptBtn)
        {
            TextView txtView = (TextView) findViewById(R.id.txtText);

            Inbox.InboxItemInfo result = Inbox.InboxItemInfo.newBuilder()
                    .setText(txtView.getText().toString())
                    .setId(Common.UniqueId.newBuilder().setGuid(guid).build())
                    .build();

            if(getIntent().getBooleanExtra("IsNewItem", false))
            {
                GlobalModel.getInboxModel().addItem(result);
            }
            else
            {
                GlobalModel.getInboxModel().modifyItem(result);
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
                    GlobalModel.getInboxModel().deleteItem(guid);
                    finish();
                }});

            dlgAlert.setCancelable(true);
            dlgAlert.create().show();
        }
    }

    private Button mBtnDelete;
    private Button mAcceptBtn;
}
