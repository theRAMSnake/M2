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
            GlobalModel.doSync();
            return p[0];
        }

        protected void onPostExecute(SyncListener listener)
        {
            listener.onSyncComplete();
        }
    }

    protected static void doSync()
    {
        mInboxModel.sync();
        mActionsModel.sync();
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

        loadState();
    }

    private static void loadState()
    {
        mInboxModel.loadState(mLocalDatabase);
        mActionsModel.loadState(mLocalDatabase);
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
        }
    }

    static private MateriaConnection mConnection;
    static private LocalDatabase mLocalDatabase;
    static private InboxModel mInboxModel;
    static private ActionsModel mActionsModel;
}
