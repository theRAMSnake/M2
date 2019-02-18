package snakesoft.minion.Activities

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import org.jetbrains.anko.*
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.SyncObserver

class SyncActivity : AppCompatActivity()
{
    val SyncObserver = snakesoft.minion.Models.SyncObserver()

    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        SyncActivityUI(SyncObserver).setContentView(this)

        doAsync()
        {
            GlobalModel.doSync(SyncObserver)
        }
    }
}

class SyncActivityUI(val SyncObserver: SyncObserver) : MateriaActivityUI<SyncActivity>()
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