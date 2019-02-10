package snakesoft.minion.Activities

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView

import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class WpActivity : AppCompatActivity(), View.OnClickListener {

    private var mBtnSuccess: Button? = null
    private var mBtnFailed: Button? = null
    private var mBtnClear: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_wp)

        val SuccessAttempts = GlobalModel.WpModel.SuccessAttempts
        val FailedAttempts = GlobalModel.WpModel.FailedAttempts

        setData(SuccessAttempts, FailedAttempts)

        mBtnSuccess = findViewById(R.id.btnSuccess) as Button
        mBtnSuccess!!.setOnClickListener(this@WpActivity)

        mBtnFailed = findViewById(R.id.btnFail) as Button
        mBtnFailed!!.setOnClickListener(this@WpActivity)

        mBtnClear = findViewById(R.id.btnClear) as Button
        mBtnClear!!.setOnClickListener(this@WpActivity)
    }

    private fun setData(SuccessAttempts: Int, FailedAttempts: Int) {
        val tv = findViewById(R.id.tvScore) as TextView
        tv.text = Integer.toString(SuccessAttempts) + ":" + Integer.toString(FailedAttempts)

        val pb = findViewById(R.id.progressBar) as ProgressBar
        pb.progress = SuccessAttempts
        pb.max = FailedAttempts + SuccessAttempts
    }

    override fun onClick(v: View) {
        if (v === mBtnSuccess) {
            GlobalModel.WpModel.SuccessAttempts = GlobalModel.WpModel.SuccessAttempts + 1

            setData(GlobalModel.WpModel.SuccessAttempts, GlobalModel.WpModel.FailedAttempts)
        }
        if (v === mBtnFailed) {
            GlobalModel.WpModel.FailedAttempts = GlobalModel.WpModel.FailedAttempts + 1

            setData(GlobalModel.WpModel.SuccessAttempts, GlobalModel.WpModel.FailedAttempts)
        }
        if (v === mBtnClear) {
            GlobalModel.WpModel.FailedAttempts = 0
            GlobalModel.WpModel.SuccessAttempts = 0
            setData(0, 0)
        }
    }
}
