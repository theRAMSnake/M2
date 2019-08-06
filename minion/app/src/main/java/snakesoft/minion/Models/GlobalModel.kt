package snakesoft.minion.Models

import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.JournalServiceProxy
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
    val FreeDataModel = FreeDataModel(Db)
    val FocusDataModel = FocusDataModel(Db)

    internal fun doSync(syncObserver: SyncObserver, password: String): Boolean
    {
        return try
        {
            val connection = MateriaConnection(Ip, password)

            //check password
            val p = JournalServiceProxy(connection)
            if(p.loadIndex().itemsCount == 0)
            {
                syncObserver.OnUpdated("Incorrect password")
                false
            }
            else
            {
                InboxModel.sync(syncObserver, connection)
                CalendarModel.sync(syncObserver, connection)
                JournalModel.sync(syncObserver, connection)
                FreeDataModel.sync(syncObserver, connection)
                FocusDataModel.sync(syncObserver, connection)

                syncObserver.finish()

                true
            }
        }
        catch (ex: Exception)
        {
            if(ex.message != null)
            {
                syncObserver.OnUpdated(ex.message!!)
            }

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
        FreeDataModel.clear()
        FocusDataModel.clear()
    }
}
