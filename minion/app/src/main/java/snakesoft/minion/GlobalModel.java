package snakesoft.minion;

import android.content.Context;
import android.os.AsyncTask;

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
    }

    public static void init(Context context)
    {
        mConnection = new MateriaConnection();
        mInboxModel = new InboxModel(new InboxServiceProxy(mConnection));
        mLocalDatabase = new LocalDatabase(context);

        loadState();
    }

    private static void loadState()
    {
        mInboxModel.loadState(mLocalDatabase);
    }

    public static void sync(SyncListener listener)
    {
        new SyncAllTask().execute(listener);
    }

    public static InboxModel getInboxModel()
    {
        return mInboxModel;
    }

    static private MateriaConnection mConnection;
    static private LocalDatabase mLocalDatabase;
    static private InboxModel mInboxModel;
}
