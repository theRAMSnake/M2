package snakesoft.minion.Activities

import android.app.AlertDialog
import android.content.DialogInterface
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.EditText
import android.widget.ProgressBar
import android.widget.TextView

import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.SyncObserver
import snakesoft.minion.R

class SyncActivity : AppCompatActivity(), SyncListener {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_sync)

        val txtView = findViewById(R.id.syncText) as EditText
        txtView.isEnabled = false
        GlobalModel.sync(this)
    }

    override fun onSyncComplete() {
        val obs = GlobalModel.syncObserver

        val pb = findViewById(R.id.pbSync) as ProgressBar
        pb.visibility = View.INVISIBLE

        val txtView = findViewById(R.id.syncText) as EditText
        txtView.setText(obs!!.log)
    }

    override fun onSyncError() {
        val dlgAlert = AlertDialog.Builder(this)
        dlgAlert.setMessage("Sync error: materia unreachable")
        dlgAlert.setTitle("Warning")
        dlgAlert.setPositiveButton("OK") { dialog, which -> finish() }

        dlgAlert.create().show()
    }
}
