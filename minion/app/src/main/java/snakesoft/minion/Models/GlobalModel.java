package snakesoft.minion.Models;

import android.content.Context;
import android.os.AsyncTask;
import android.widget.Toast;

import java.io.UnsupportedEncodingException;
import java.util.Vector;

import snakesoft.minion.Materia.InboxServiceProxy;
import snakesoft.minion.Materia.CalendarServiceProxy;
import snakesoft.minion.Materia.MateriaConnection;
import snakesoft.minion.Materia.MateriaUnreachableException;
import snakesoft.minion.Activities.SyncListener;

/**
 * Created by snake on 4/28/17.
 */

public class GlobalModel
{
    public static SyncObserver getSyncObserver()
    {
        return mSyncObserver;
    }

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
            mSyncObserver = new SyncObserver();

            mInboxModel.sync(mSyncObserver);
            mCalendarModel.sync(mSyncObserver);

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
        mWpModel = new WpModel();
        mCalendarModel = new CalendarModel(
                new CalendarServiceProxy(mConnection));

        loadState(context);
    }

    private static void loadState(Context context)
    {
        boolean dbok = true;
        try
        {
            mInboxModel.loadState(mLocalDatabase);
        } catch (Exception e)
        {
            dbok = false;
        }
        try
        {
            mCalendarModel.loadState(mLocalDatabase);
        } catch (Exception e)
        {
            dbok = false;
        }

        mWpModel.loadState(mLocalDatabase);

        if(!dbok)
        {

            CharSequence text = "Database is inconsistent please reset.";
            int duration = Toast.LENGTH_LONG;

            Toast toast = Toast.makeText(context, text, duration);
            toast.show();
            return;
        }
    }

    public static void sync(SyncListener listener)
    {
        new SyncAllTask().execute(listener);
    }

    public static InboxModel getInboxModel()
    {
        return mInboxModel;
    }
    public static CalendarModel getCalendarModel() {return mCalendarModel; }
    public static WpModel getWpModel() {return mWpModel;}

    public static String getIp()
    {
        return mConnection.getIp();
    }

    public static void reset()
    {
        mInboxModel.resetChanges();
        mCalendarModel.resetChanges();
    }

    public static void setNewIp(String ip)
    {
        if(!mConnection.getIp().equals(ip))
        {
            mLocalDatabase.put("Settings.IP", ip.getBytes());
            mConnection.setNewIp(ip);
            reset();
        }
    }

    static private MateriaConnection mConnection;
    static private LocalDatabase mLocalDatabase;
    static private InboxModel mInboxModel;
    static private CalendarModel mCalendarModel;
    static private WpModel mWpModel;
    static private SyncObserver mSyncObserver;
}
