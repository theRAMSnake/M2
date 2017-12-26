package snakesoft.minion;

import android.content.Context;
import android.os.AsyncTask;

import java.io.UnsupportedEncodingException;

/**
 * Created by snake on 4/28/17.
 */

public class GlobalModel
{
    static class SyncAllTask extends AsyncTask<SyncListener, Void, SyncListener>
    {
        protected SyncListener doInBackground(SyncListener... p)
        {
            mSyncResult = GlobalModel.doSync();
            return p[0];
        }

        protected void onPostExecute(SyncListener listener)
        {
            if(mSyncResult)
            {
                listener.onSyncComplete();
            }
            else
            {
                listener.onSyncError();
            }
        }

        private boolean mSyncResult;
    }

    protected static boolean doSync()
    {
        try
        {
            mInboxModel.sync();
            mActionsModel.sync();
            mCalendarModel.sync();

            return true;
        }
        catch(MateriaUnreachableException ex)
        {
            return false;
        }
    }

    public static void init(Context context)
    {
        mLocalDatabase = new LocalDatabase(context);

        byte[] ipbytes = mLocalDatabase.get("Settings.IP");
        if(ipbytes != null)
        {
            String ip = null;
            try
            {
                ip = new String(ipbytes, "UTF-8");
            }
            catch (UnsupportedEncodingException e)
            {

            }
            mConnection = new MateriaConnection(ip);
        }
        else
        {
            mConnection = new MateriaConnection("localhost");
        }

        mInboxModel = new InboxModel(new InboxServiceProxy(mConnection));
        mActionsModel = new ActionsModel(new ActionsServiceProxy(mConnection));
        mCalendarModel = new CalendarModel(
                new CalendarServiceProxy(mConnection));

        loadState();
    }

    private static void loadState()
    {
        mInboxModel.loadState(mLocalDatabase);
        mActionsModel.loadState(mLocalDatabase);
        mCalendarModel.loadState(mLocalDatabase);
    }

    public static void sync(SyncListener listener)
    {
        new SyncAllTask().execute(listener);
    }

    public static InboxModel getInboxModel()
    {
        return mInboxModel;
    }
    public static ActionsModel getActionsModel() {return mActionsModel; }
    public static CalendarModel getCalendarModel() {return mCalendarModel; }

    public static String getIp()
    {
        return mConnection.getIp();
    }

    public static void setNewIp(String ip)
    {
        if(!mConnection.getIp().equals(ip))
        {
            mLocalDatabase.put("Settings.IP", ip.getBytes());
            mConnection.setNewIp(ip);
            mInboxModel.resetChanges();
            mActionsModel.resetChanges();
            mCalendarModel.resetChanges();
        }
    }

    static private MateriaConnection mConnection;
    static private LocalDatabase mLocalDatabase;
    static private InboxModel mInboxModel;
    static private ActionsModel mActionsModel;
    static private CalendarModel mCalendarModel;
}
