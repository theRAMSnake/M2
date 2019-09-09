package snakesoft.minion.Activities

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.provider.Telephony
import android.util.Log
import kotlinx.coroutines.GlobalScope
import org.jetbrains.anko.AnkoViewDslMarker
import org.jetbrains.anko.backgroundColor
import org.jetbrains.anko.setContentView
import org.jetbrains.anko.support.v4._DrawerLayout

class MainActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        MainActivityUI().setContentView(this)
    }
}

class MainActivityUI : MateriaActivityUI<MainActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        _DrawerLayout.backgroundColor = Color.CYAN
    }
}