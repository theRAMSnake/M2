package snakesoft.minion.Activities

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button

import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R


class MainActivity : AppCompatActivity(), View.OnClickListener {

    private var mSyncBtn: Button? = null
    private var mInboxBtn: Button? = null
    private var mActionsBtn: Button? = null
    private var mSettingsBtn: Button? = null
    private var mCalendarBtn: Button? = null
    private val mBacklogBtn: Button? = null
    private var mWpBtn: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        mSyncBtn = findViewById(R.id.btnSync) as Button
        mSyncBtn!!.setOnClickListener(this@MainActivity)

        mInboxBtn = findViewById(R.id.btnInbox) as Button
        mInboxBtn!!.setOnClickListener(this@MainActivity)

        mActionsBtn = findViewById(R.id.btnActions) as Button
        mActionsBtn!!.setOnClickListener(this@MainActivity)

        mSettingsBtn = findViewById(R.id.btnSettings) as Button
        mSettingsBtn!!.setOnClickListener(this@MainActivity)

        mCalendarBtn = findViewById(R.id.btnCalendar) as Button
        mCalendarBtn!!.setOnClickListener(this@MainActivity)

        mWpBtn = findViewById(R.id.btnWp) as Button
        mWpBtn!!.setOnClickListener(this@MainActivity)

        GlobalModel.init(applicationContext)
    }

    override fun onClick(v: View) {
        if (v === mInboxBtn) {
            val myIntent = Intent(this@MainActivity, InboxActivity::class.java)
            this@MainActivity.startActivity(myIntent)
        }
        if (v === mSyncBtn) {
            val myIntent = Intent(this@MainActivity, SyncActivity::class.java)
            this@MainActivity.startActivity(myIntent)
        }
        if (v === mActionsBtn) {
            val myIntent = Intent(this@MainActivity, ActionsActivity::class.java)
            myIntent.putExtra("Mode", "Normal")
            this@MainActivity.startActivity(myIntent)
        }
        if (v === mSettingsBtn) {
            val myIntent = Intent(this@MainActivity, SettingsActivity::class.java)
            this@MainActivity.startActivity(myIntent)
        }
        if (v === mCalendarBtn) {
            val myIntent = Intent(this@MainActivity, CalendarActivity::class.java)
            startActivity(myIntent)
        }
        if (v === mBacklogBtn) {
            val myIntent = Intent(this@MainActivity, ActionsActivity::class.java)
            myIntent.putExtra("Mode", "Backlog")
            this@MainActivity.startActivity(myIntent)
        }
        if (v === mWpBtn) {
            val myIntent = Intent(this@MainActivity, WpActivity::class.java)
            this@MainActivity.startActivity(myIntent)
        }
    }
}
