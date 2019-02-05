package snakesoft.minion.Models

import android.content.Context
import android.os.AsyncTask
import android.widget.Toast

import snakesoft.minion.materia.InboxServiceProxy
import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import snakesoft.minion.Activities.SyncListener

/**
 * Created by snake on 4/28/17.
 */

object GlobalModel {

    val ip: String?
        get() = mConnection!!.ip

    private var mConnection: MateriaConnection? = null
    private var mLocalDatabase: LocalDatabase? = null
    var inboxModel: InboxModel? = null
        private set
    var calendarModel: CalendarModel? = null
        private set
    var wpModel: WpModel? = null
        private set
    var syncObserver: SyncObserver? = null
        private set

    internal class SyncAllTask : AsyncTask<SyncListener, Void, SyncListener>() {

        private var mSyncResult: Boolean = false
        override fun doInBackground(vararg p: SyncListener): SyncListener {
            mSyncResult = GlobalModel.doSync()
            return p[0]
        }

        override fun onPostExecute(listener: SyncListener) {
            if (mSyncResult) {
                listener.onSyncComplete()
            } else {
                listener.onSyncError()
            }
        }
    }

    internal fun doSync(): Boolean {
        try {
            var newSyncObserver = SyncObserver()

            inboxModel!!.sync(newSyncObserver)
            calendarModel!!.sync(newSyncObserver)

            syncObserver = newSyncObserver

            return true
        } catch (ex: MateriaUnreachableException) {
            return false
        }

    }

    fun init(context: Context) {
        mLocalDatabase = LocalDatabase(context)

        val ipbytes = mLocalDatabase!!.get("Settings.IP")
        if (ipbytes != null)
        {
            var ip = String(ipbytes)

            mConnection = MateriaConnection(ip)
        }
        else
        {
            mConnection = MateriaConnection("localhost")
        }

        var connection = mConnection;

        inboxModel = InboxModel(InboxServiceProxy(connection!!))
        wpModel = WpModel()
        calendarModel = CalendarModel(
                CalendarServiceProxy(connection!!))

        loadState(context)
    }

    private fun loadState(context: Context) {
        var dbok = true
        try {
            inboxModel!!.loadState(mLocalDatabase!!)
        } catch (e: Exception) {
            dbok = false
        }

        try {
            calendarModel!!.loadState(mLocalDatabase!!)
        } catch (e: Exception) {
            dbok = false
        }

        wpModel!!.loadState(mLocalDatabase!!)

        if (!dbok) {

            val text = "Database is inconsistent please reset."
            val duration = Toast.LENGTH_LONG

            val toast = Toast.makeText(context, text, duration)
            toast.show()
            return
        }
    }

    fun sync(listener: SyncListener) {
        SyncAllTask().execute(listener)
    }

    fun reset() {
        inboxModel!!.resetChanges()
        calendarModel!!.resetChanges()
    }

    fun setNewIp(ip: String) {
        if (mConnection!!.ip != ip) {
            mLocalDatabase!!.put("Settings.IP", ip.toByteArray())
            mConnection!!.setNewIp(ip)
            reset()
        }
    }
}
