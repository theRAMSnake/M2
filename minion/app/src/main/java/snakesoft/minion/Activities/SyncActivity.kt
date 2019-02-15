package snakesoft.minion.Activities

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import org.jetbrains.anko.AnkoViewDslMarker
import org.jetbrains.anko.setContentView
import org.jetbrains.anko.support.v4._DrawerLayout
import org.jetbrains.anko.textView

class SyncActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        SyncActivityUI().setContentView(this)
    }
}

class SyncActivityUI : MateriaActivityUI<SyncActivity>()
{
    override fun fillActivityUI(_DrawerLayout:@AnkoViewDslMarker _DrawerLayout)
    {
        with(_DrawerLayout)
        {
            textView("sss")
        }
    }
}