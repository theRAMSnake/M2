package snakesoft.minion.Models

import android.content.Context
import android.os.AsyncTask
import android.widget.Toast

import snakesoft.minion.materia.InboxServiceProxy
import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import snakesoft.minion.Activities.SyncListener

object GlobalModel
{
    var Ip: String = ""
        set(value)
        {
            field = value
            Db.put("Settings.IP", value)
            reset()
        }

    private val Db = LocalDatabase()
    val InboxModel = InboxModel(Db)
    val CalendarModel = CalendarModel(Db)
    val WpModel = WpModel(Db)
    var LastSyncLog = ""

    internal class SyncAllTask : AsyncTask<SyncListener, Void, SyncListener>()
    {
        private var mSyncResult: Boolean = false
        override fun doInBackground(vararg p: SyncListener): SyncListener
        {
            mSyncResult = GlobalModel.doSync()
            return p[0]
        }

        override fun onPostExecute(listener: SyncListener)
        {
            if (mSyncResult) {
                listener.onSyncComplete()
            } else {
                listener.onSyncError()
            }
        }
    }

    internal fun doSync(): Boolean
    {
        return try
        {
            val newSyncObserver = SyncObserver()
            val connection = MateriaConnection(Ip)

            InboxModel.sync(newSyncObserver)
            CalendarModel.sync(newSyncObserver, connection)

            LastSyncLog = newSyncObserver.Log

            true
        }
        catch (ex: MateriaUnreachableException)
        {
            false
        }
    }

    init
    {
        Ip = Db["Settings.IP"]
    }

    fun sync(listener: SyncListener)
    {
        SyncAllTask().execute(listener)
    }

    fun reset()
    {
        InboxModel.resetChanges()
        CalendarModel.clear()
    }
}
