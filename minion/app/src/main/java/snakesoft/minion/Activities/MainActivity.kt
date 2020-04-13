package snakesoft.minion.Activities

import android.content.*
import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.provider.Telephony
import android.util.Log
import android.widget.Toast
import kotlinx.coroutines.GlobalScope
import org.jetbrains.anko.AnkoViewDslMarker
import org.jetbrains.anko.backgroundColor
import org.jetbrains.anko.design.longSnackbar
import org.jetbrains.anko.setContentView
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.ClipboardListenerService
import snakesoft.minion.Models.Event1

class MainActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)

        val intent = Intent(applicationContext, ClipboardListenerService::class.java)
        startService(intent)
        startForegroundService(intent)

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