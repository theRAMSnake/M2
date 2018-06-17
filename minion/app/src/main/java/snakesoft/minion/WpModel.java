package snakesoft.minion;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

/**
 * Created by snake on 6/10/18.
 */

public class WpModel
{
    public void loadState(LocalDatabase localDb) {
        mLocalDb = localDb;
        try
        {
            ByteArrayInputStream byteStream = new ByteArrayInputStream(localDb.get("WillPowerTracking"));
            mSuccessAtempts = byteStream.read();
            mFailedAtempts = byteStream.read();
        }
        catch (Exception ex)
        {
            mSuccessAtempts = 0;
            mFailedAtempts = 0;
        }
    }

    public void saveState()
    {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        bos.write(mSuccessAtempts);
        bos.write(mFailedAtempts);

        mLocalDb.put("WillPowerTracking", bos.toByteArray());
    }

    public int getSuccessAtempts()
    {
        return mSuccessAtempts;
    }

    public int getFailedAtempts()
    {
        return mFailedAtempts;
    }

    public void setSuccessAtempts(int newValue)
    {
        mSuccessAtempts = newValue;
        saveState();
    }

    public void setFailedAtempts(int newValue)
    {
        mFailedAtempts = newValue;
        saveState();
    }

    private LocalDatabase mLocalDb;
    private int mSuccessAtempts = 0;
    private int mFailedAtempts = 0;
}
