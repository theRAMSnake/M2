package snakesoft.minion.Activities

import android.app.AlertDialog
import android.content.Context
import android.content.DialogInterface
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.view.inputmethod.InputMethodManager
import android.widget.Button
import android.widget.TextView

import common.Common
import inbox.Inbox
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class InboxItemViewActivity : AppCompatActivity(), View.OnClickListener {

    private var mBtnDelete: Button? = null
    private var mAcceptBtn: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_inbox_item_view)

        mAcceptBtn = findViewById(R.id.btnAccept) as Button
        mAcceptBtn!!.setOnClickListener(this@InboxItemViewActivity)

        mBtnDelete = findViewById(R.id.btnDelete) as Button
        mBtnDelete!!.setOnClickListener(this@InboxItemViewActivity)

        if (intent.getBooleanExtra("IsNewItem", false)) {
            mBtnDelete!!.visibility = View.INVISIBLE
            val txtView = findViewById(R.id.txtText) as TextView
            txtView.requestFocus()
            val imm = getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
            imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY)
        } else {
            val txtView = findViewById(R.id.txtText) as TextView
            txtView.text = intent.getStringExtra("Text")
        }
    }

    override fun onClick(v: View) {
        val guid = intent.getStringExtra("Id")
        if (v === mAcceptBtn) {
            val txtView = findViewById(R.id.txtText) as TextView

            val result = Inbox.InboxItemInfo.newBuilder()
                    .setText(txtView.text.toString())
                    .setId(Common.UniqueId.newBuilder().setGuid(guid).build())
                    .build()

            if (intent.getBooleanExtra("IsNewItem", false)) {
                GlobalModel.inboxModel!!.addItem(result)
            } else {
                GlobalModel.inboxModel!!.modifyItem(result)
            }

            finish()
        }
        if (v === mBtnDelete) {
            val dlgAlert = AlertDialog.Builder(this)
            dlgAlert.setMessage("Are you sure?")
            dlgAlert.setTitle("Caution")
            dlgAlert.setPositiveButton("OK") { dialog, which ->
                GlobalModel.inboxModel!!.deleteItem(guid)
                finish()
            }

            dlgAlert.setCancelable(true)
            dlgAlert.create().show()
        }
    }
}
