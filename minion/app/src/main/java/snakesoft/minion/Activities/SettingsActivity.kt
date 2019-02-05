package snakesoft.minion.Activities

import android.app.AlertDialog
import android.content.DialogInterface
import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.TextView

import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class SettingsActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_settings)

        val txtView = findViewById(R.id.edtIP) as TextView
        txtView.setText(GlobalModel.ip)

        (findViewById(R.id.btnSave) as Button).setOnClickListener {
            val ip = txtView.text.toString()
            GlobalModel.setNewIp(ip)

            val myIntent = Intent(this@SettingsActivity, SyncActivity::class.java)
            this@SettingsActivity.startActivity(myIntent)
        }

        (findViewById(R.id.set_btnReset) as Button).setOnClickListener {
            val dlgAlert = AlertDialog.Builder(this@SettingsActivity)
            dlgAlert.setMessage("Are you sure?")
            dlgAlert.setTitle("Caution")
            dlgAlert.setPositiveButton("OK") { dialog, which ->
                GlobalModel.reset()

                val myIntent = Intent(this@SettingsActivity, SyncActivity::class.java)
                this@SettingsActivity.startActivity(myIntent)
            }

            dlgAlert.setCancelable(true)
            dlgAlert.create().show()
        }
    }
}
