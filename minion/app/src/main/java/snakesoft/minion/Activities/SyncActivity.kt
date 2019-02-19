package snakesoft.minion.Activities

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import org.jetbrains.anko.*
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.SyncObserver

class SyncActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        val syncObserver = snakesoft.minion.Models.SyncObserver()
        super.onCreate(savedInstanceState)
        SyncActivityUI(syncObserver).setContentView(this)

        doAsync()
        {
            GlobalModel.doSync(syncObserver)
        }
    }
}

class SyncActivityUI(private val SyncObserver: SyncObserver) : MateriaActivityUI<SyncActivity>()
{
    override fun fillActivityUI(_DrawerLayout:@AnkoViewDslMarker _DrawerLayout)
    {
        with(_DrawerLayout)
        {
            textView()
            {
                singleLine = false

                SyncObserver.OnFinished += {
                    append(SyncObserver.Log)
                }
            }
        }
    }
}