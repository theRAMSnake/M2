package snakesoft.minion.Models

import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import snakesoft.minion.materia.RewardServiceProxy

class RewardModel(private val Db: LocalDatabase)
{
    var Points: UInt = 0u
        set(value) {field = value; saveState()}

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("Reward")

        val proxy = RewardServiceProxy(connection)
        proxy.addPoints(Points)

        Points = 0u

        observer.endSync()
    }

    private fun saveState()
    {
        Db.put("Points", Points.toString())
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Points = Db["Points"].toUInt()
        }
        catch(ex: Exception)
        {

        }
    }

    fun clear()
    {
        Points = 0u
    }
}