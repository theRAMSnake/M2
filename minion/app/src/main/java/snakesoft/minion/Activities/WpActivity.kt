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

        val successAtempts = GlobalModel.wpModel!!.successAtempts
        val failedAtempts = GlobalModel.wpModel!!.failedAtempts

        setData(successAtempts, failedAtempts)

        mBtnSuccess = findViewById(R.id.btnSuccess) as Button
        mBtnSuccess!!.setOnClickListener(this@WpActivity)

        mBtnFailed = findViewById(R.id.btnFail) as Button
        mBtnFailed!!.setOnClickListener(this@WpActivity)

        mBtnClear = findViewById(R.id.btnClear) as Button
        mBtnClear!!.setOnClickListener(this@WpActivity)
    }

    private fun setData(successAtempts: Int, failedAtempts: Int) {
        val tv = findViewById(R.id.tvScore) as TextView
        tv.text = Integer.toString(successAtempts) + ":" + Integer.toString(failedAtempts)

        val pb = findViewById(R.id.progressBar) as ProgressBar
        pb.progress = successAtempts
        pb.max = failedAtempts + successAtempts
    }

    override fun onClick(v: View) {
        if (v === mBtnSuccess) {
            GlobalModel.wpModel!!.successAtempts = GlobalModel.wpModel!!.successAtempts + 1

            setData(GlobalModel.wpModel!!.successAtempts, GlobalModel.wpModel!!.failedAtempts)
        }
        if (v === mBtnFailed) {
            GlobalModel.wpModel!!.failedAtempts = GlobalModel.wpModel!!.failedAtempts + 1

            setData(GlobalModel.wpModel!!.successAtempts, GlobalModel.wpModel!!.failedAtempts)
        }
        if (v === mBtnClear) {
            GlobalModel.wpModel!!.failedAtempts = 0
            GlobalModel.wpModel!!.successAtempts = 0
            setData(0, 0)
        }
    }
}
