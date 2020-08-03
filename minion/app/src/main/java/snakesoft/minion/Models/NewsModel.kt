package snakesoft.minion.Models

import snakesoft.minion.materia.MateriaConnection
import snakesoft.minion.materia.MateriaUnreachableException
import snakesoft.minion.materia.NewsServiceProxy
import snakesoft.minion.materia.RewardServiceProxy

class NewsModel(private val Db: LocalDatabase)
{
    private var Content: String = ""

    init
    {
        loadState()
    }

    @Throws(MateriaUnreachableException::class)
    fun sync(observer: SyncObserver, connection: MateriaConnection)
    {
        observer.beginSync("News")

        val proxy = NewsServiceProxy(connection)
        Content = proxy.load()
        saveState()

        observer.endSync()
    }

    private fun saveState()
    {
        Db.put("News", Content)
    }

    @Throws(Exception::class)
    private fun loadState()
    {
        try
        {
            Content = Db["News"].toString()
        }
        catch(ex: Exception)
        {

        }
    }

    fun get(): String
    {
        return Content
    }

    fun clear()
    {
        Content = ""
    }
}