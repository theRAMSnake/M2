package snakesoft.minion.Models

import android.os.AsyncTask

import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException

object GlobalModel
{
    var Ip: String = "188.116.57.62"
        set(value)
        {
            field = value
            Db.put("Settings.IP", value)
            reset()
        }

    private val Db = LocalDatabase()
    val InboxModel = InboxModel(Db)
    val CalendarModel = CalendarModel(Db)
    val JournalModel = JournalModel(Db)
    val StrategyModel = StrategyModel(Db)

    internal fun doSync(syncObserver: SyncObserver): Boolean
    {
        return try
        {
            val connection = MateriaConnection(Ip)

            InboxModel.sync(syncObserver, connection)
            CalendarModel.sync(syncObserver, connection)
            JournalModel.sync(syncObserver, connection)
            StrategyModel.sync(syncObserver, connection)

            syncObserver.finish()

            true
        }
        catch (ex: MateriaUnreachableException)
        {
            false
        }
    }

    init
    {
        //Ip = Db["Settings.IP"]
    }

    fun reset()
    {
        InboxModel.clear()
        CalendarModel.clear()
        JournalModel.clear()
        StrategyModel.clear()
    }
}