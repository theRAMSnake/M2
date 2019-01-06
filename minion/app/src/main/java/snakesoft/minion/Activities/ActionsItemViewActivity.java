package snakesoft.minion.Activities;

import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.TextView;

import snakesoft.minion.R;

public class ActionsItemViewActivity extends AppCompatActivity implements View.OnClickListener
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_actions_item_view);

        mAcceptBtn = (Button) findViewById(R.id.action_item_accept);
        mAcceptBtn.setOnClickListener(ActionsItemViewActivity.this);

        mBtnDelete = (Button) findViewById(R.id.action_item_delete);
        mBtnDelete.setOnClickListener(ActionsItemViewActivity.this);

        if(getIntent().getBooleanExtra("IsNewItem", false))
        {
            mBtnDelete.setVisibility(View.INVISIBLE);

            CharSequence colors[] = new CharSequence[] {"Task", "Group"};

            android.support.v7.app.AlertDialog.Builder builder = new android.support.v7.app.AlertDialog.Builder(this);
            builder.setTitle("Choose new item type");
            builder.setItems(colors, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which)
                {
                    mNewItemType = which;
                }
            });
            builder.show();

            TextView txtView = (TextView) findViewById(R.id.action_item_title);
            txtView.requestFocus();
            InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY);
        }
        else
        {
            TextView txtView = (TextView) findViewById(R.id.action_item_title);
            txtView.setText(getIntent().getStringExtra("Title"));

            WebView txtView2 = (WebView) findViewById(R.id.action_item_description);
            String str = "<html><body>" + getIntent().getStringExtra("Desc") + "</body></html>";
            //String str = "<html><body>" + "adjkfksdkjfk" + "</body></html>";
            txtView2.loadData(str, "text/html", "UTF-8");
        }
    }

    public void onClick(View v)
    {
        /*final String guid = getIntent().getStringExtra("Id");
        final String parentGuid = getIntent().getStringExtra("ParentId");
        final int itemType = getIntent().getBooleanExtra("IsNewItem", false)
                ? mNewItemType
                : getIntent().getIntExtra("Type", 0);

        if(v == mAcceptBtn)
        {
            TextView txtTitle = (TextView) findViewById(R.id.action_item_title);

            Actions.ActionInfo.Builder b = Actions.ActionInfo.newBuilder()
                    .setTitle(txtTitle.getText().toString())
                    .setDescription(getIntent().getBooleanExtra("IsNewItem", false) ? "" : getIntent().getStringExtra("Desc"))
                    .setId(Common.UniqueId.newBuilder().setGuid(guid).build())
                    .setType(Actions.ActionType.values()[itemType]);

            if(parentGuid != null)
            {
                b.setParentId(Common.UniqueId.newBuilder().setGuid(parentGuid).build());
            }

            Actions.ActionInfo result = b.build();

            if(getIntent().getBooleanExtra("IsNewItem", false))
            {
                GlobalModel.getActionsModel().addItem(result);
            }
            else
            {
                GlobalModel.getActionsModel().modifyItem(result);
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
                    GlobalModel.getActionsModel().deleteItem(guid);
                    finish();
                }});

            dlgAlert.setCancelable(true);
            dlgAlert.create().show();
        }*/
    }

    private int mNewItemType = 1;
    private Button mBtnDelete;
    private Button mAcceptBtn;
}
