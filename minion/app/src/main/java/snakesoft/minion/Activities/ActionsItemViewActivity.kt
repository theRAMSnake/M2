package snakesoft.minion.Activities

import android.content.Context
import android.content.DialogInterface
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.view.inputmethod.InputMethodManager
import android.webkit.WebView
import android.widget.Button
import android.widget.TextView

import snakesoft.minion.R

class ActionsItemViewActivity : AppCompatActivity(), View.OnClickListener {

    private var mNewItemType = 1
    private var mBtnDelete: Button? = null
    private var mAcceptBtn: Button? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_actions_item_view)

        mAcceptBtn = findViewById(R.id.action_item_accept) as Button
        mAcceptBtn!!.setOnClickListener(this@ActionsItemViewActivity)

        mBtnDelete = findViewById(R.id.action_item_delete) as Button
        mBtnDelete!!.setOnClickListener(this@ActionsItemViewActivity)

        if (intent.getBooleanExtra("IsNewItem", false)) {
            mBtnDelete!!.visibility = View.INVISIBLE

            val colors = arrayOf<CharSequence>("Task", "Group")

            val builder = android.support.v7.app.AlertDialog.Builder(this)
            builder.setTitle("Choose new item type")
            builder.setItems(colors) { dialog, which -> mNewItemType = which }
            builder.show()

            val txtView = findViewById(R.id.action_item_title) as TextView
            txtView.requestFocus()
            val imm = getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
            imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY)
        } else {
            val txtView = findViewById(R.id.action_item_title) as TextView
            txtView.text = intent.getStringExtra("Title")

            val txtView2 = findViewById(R.id.action_item_description) as WebView
            val str = "<html><body>" + intent.getStringExtra("Desc") + "</body></html>"
            //String str = "<html><body>" + "adjkfksdkjfk" + "</body></html>";
            txtView2.loadData(str, "text/html", "UTF-8")
        }
    }

    override fun onClick(v: View) {
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
}
