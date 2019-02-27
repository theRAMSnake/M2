package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import org.jetbrains.anko.*
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.Event1
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.SyncObserver

class SyncActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        val syncObserver = snakesoft.minion.Models.SyncObserver()
        super.onCreate(savedInstanceState)

        val onSyncFinished = Event1<String>()
        SyncActivityUI(onSyncFinished).setContentView(this)

        doAsync()
        {
            GlobalModel.doSync(syncObserver)
            uiThread()
            {
                onSyncFinished(syncObserver.Log)
            }
        }
    }
}

class SyncActivityUI(private val onSyncFinished: Event1<String>) : MateriaActivityUI<SyncActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            textView()
            {
                singleLine = false
                onSyncFinished += { s -> append(s) }
            }
        }
    }
}