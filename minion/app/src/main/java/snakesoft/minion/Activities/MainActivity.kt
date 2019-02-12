package snakesoft.minion.Activities

import android.content.res.ColorStateList
import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.Gravity
import org.jetbrains.anko.*
import org.jetbrains.anko.design.navigationView
import org.jetbrains.anko.support.v4.drawerLayout


class MainActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        MainActivityUI().setContentView(this)
    }
}

fun createStateColors(): ColorStateList
{
    var states = arrayOf(intArrayOf(android.R.attr.state_enabled), // enabled
            intArrayOf(-android.R.attr.state_enabled), // disabled
            intArrayOf(-android.R.attr.state_checked), // unchecked
            intArrayOf(android.R.attr.state_pressed)  // pressed
    )

    var colors = intArrayOf(Color.WHITE, Color.WHITE, Color.WHITE, Color.WHITE)

    return ColorStateList(states, colors)
}


class MainActivityUI : AnkoComponent<MainActivity>
{
    override fun createView(ui: AnkoContext<MainActivity>) = with(ui)
    {
        linearLayout()
        {
            lparams(width = matchParent, height = matchParent)

            drawerLayout()
            {
                lparams(width = matchParent, height = matchParent, gravity = Gravity.START)
                foregroundGravity = Gravity.START

                navigationView()
                {
                    backgroundColor = Color.rgb(88, 88, 88)
                    itemTextColor = createStateColors()

                    menu.add("Sync")
                    menu.add("Inbox")
                    menu.add("Calendar")
                    menu.add("Settings")
                }.lparams(width = wrapContent, height = matchParent, gravity = Gravity.START)

                //items here
            }
        }
    }
}