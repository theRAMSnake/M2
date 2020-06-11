package snakesoft.minion.Models

import snakesoft.minion.materia.CalendarServiceProxy
import snakesoft.minion.materia.JournalServiceProxy
import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException

object GlobalModel
{
    var Ip: String = "62.171.175.23"
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
    val FinanceModel = FinanceModel(Db)
    val RewardModel = RewardModel(Db)

    internal fun doSync(syncObserver: SyncObserver, password: String): Boolean
    {
        return try
        {
            val connection = MateriaConnection(Ip, password, "5757")

            //check password
            val p = JournalServiceProxy(connection)
            if(p.loadIndex().itemsCount == 0)
            {
                syncObserver.OnUpdated("Incorrect password")
                false
            }
            else
            {
                CalendarModel.sync(syncObserver, connection)
                JournalModel.sync(syncObserver, connection)
                FinanceModel.sync(syncObserver, connection)
                RewardModel.sync(syncObserver, connection)

                val connectionNew = MateriaConnection(Ip, password, "5756")
                InboxModel.sync(syncObserver, connectionNew)

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
        FinanceModel.clear()
        RewardModel.clear()
    }
}
