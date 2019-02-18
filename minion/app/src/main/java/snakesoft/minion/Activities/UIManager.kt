package snakesoft.minion.Activities
import android.app.Activity
import android.view.Menu
import org.jetbrains.anko.*

object UIManager
{
    fun <T> fillNavigationMenu(menu: Menu, c: AnkoContext<T>)
    {
        addMenuItem(menu, "Sync", c)

        //menu.add("Inbox")
        //menu.add("Calendar")
        //menu.add("Settings")
    }

    private fun <T> addMenuItem(menu: Menu, text: String, c: AnkoContext<T>)
    {
        val context = c.ctx
        if(context is Activity)
        {
            menu.add(text).setOnMenuItemClickListener()
            {
                context.finish()
                c.startActivity<SyncActivity>()
                true
            }
        }
    }
}