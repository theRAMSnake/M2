package snakesoft.minion.Activities
import android.app.Activity
import android.view.Menu
import org.jetbrains.anko.*

object UIManager
{
    fun <T> fillNavigationMenu(menu: Menu, c: AnkoContext<T>)
    {
        addMenuItem<SyncPrepareActivity, T>(menu, "Sync", c)
        addMenuItem<InboxActivity, T>(menu, "Inbox", c)
        addMenuItem<CalendarActivity, T>(menu, "Calendar", c)
        addMenuItem<JournalActivity, T>(menu, "Journal", c)
        addMenuItem<FinanceActivity, T>(menu, "Finance", c)
        addMenuItem<RewardActivity, T>(menu, "Reward", c)
        addMenuItem<NewsActivity, T>(menu, "News", c)
    }

    private inline fun <reified A : Activity, T> addMenuItem(menu: Menu, text: String, c: AnkoContext<T>)
    {
        val context = c.ctx
        if(context is Activity)
        {
            menu.add(text).setOnMenuItemClickListener()
            {
                context.finish()
                c.startActivity<A>()
                true
            }
        }
    }
}