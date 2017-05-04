package snakesoft.minion;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
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

        TextView txtView = (TextView) findViewById(R.id.txtText);
        txtView.setText(getIntent().getStringExtra("Text"));

        mAcceptBtn = (Button) findViewById(R.id.btnAccept);
        mAcceptBtn.setOnClickListener(InboxItemViewActivity.this);

        mBtnDelete = (Button) findViewById(R.id.btnDelete);
        mBtnDelete.setOnClickListener(InboxItemViewActivity.this);
    }

    @Override
    public void onClick(View v)
    {
        if(v == mAcceptBtn)
        {
            TextView txtView = (TextView) findViewById(R.id.txtText);
            String guid = getIntent().getStringExtra("Id");

            Inbox.InboxItemInfo result = Inbox.InboxItemInfo.newBuilder()
                    .setText(txtView.getText().toString())
                    .setId(Common.UniqueId.newBuilder().setGuid(guid).build())
                    .build();

            GlobalModel.getInboxModel().modifyItem(result);
            finish();
        }
        if(v == mBtnDelete)
        {

        }
    }

    private Button mBtnDelete;
    private Button mAcceptBtn;
}
