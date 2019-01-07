package snakesoft.minion.Activities;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import snakesoft.minion.Models.GlobalModel;
import snakesoft.minion.R;

public class WpActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wp);

        int successAtempts = GlobalModel.getWpModel().getSuccessAtempts();
        int failedAtempts = GlobalModel.getWpModel().getFailedAtempts();

        setData(successAtempts, failedAtempts);

        mBtnSuccess = (Button) findViewById(R.id.btnSuccess);
        mBtnSuccess.setOnClickListener(WpActivity.this);

        mBtnFailed = (Button) findViewById(R.id.btnFail);
        mBtnFailed.setOnClickListener(WpActivity.this);

        mBtnClear = (Button) findViewById(R.id.btnClear);
        mBtnClear.setOnClickListener(WpActivity.this);
    }

    private void setData(int successAtempts, int failedAtempts) {
        TextView tv = (TextView) findViewById(R.id.tvScore);
        tv.setText(Integer.toString(successAtempts) + ":" + Integer.toString(failedAtempts));

        ProgressBar pb = (ProgressBar) findViewById(R.id.progressBar);
        pb.setProgress(successAtempts);
        pb.setMax(failedAtempts + successAtempts);
    }

    @Override
    public void onClick(View v)
    {
        if(v == mBtnSuccess)
        {
            GlobalModel.getWpModel().setSuccessAtempts(
                    GlobalModel.getWpModel().getSuccessAtempts() + 1
            );

            setData(GlobalModel.getWpModel().getSuccessAtempts(), GlobalModel.getWpModel().getFailedAtempts());
        }
        if(v == mBtnFailed)
        {
            GlobalModel.getWpModel().setFailedAtempts(
                    GlobalModel.getWpModel().getFailedAtempts() + 1
            );

            setData(GlobalModel.getWpModel().getSuccessAtempts(), GlobalModel.getWpModel().getFailedAtempts());
        }
        if(v == mBtnClear)
        {
            GlobalModel.getWpModel().setFailedAtempts(0);
            GlobalModel.getWpModel().setSuccessAtempts(0);
            setData(0, 0);
        }
    }

    private Button mBtnSuccess;
    private Button mBtnFailed;
    private Button mBtnClear;
}
