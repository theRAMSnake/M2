package snakesoft.minion.Activities

import android.content.res.ColorStateList
import android.graphics.Color
import android.view.Gravity
import org.jetbrains.anko.*
import org.jetbrains.anko.design.navigationView
import org.jetbrains.anko.support.v4._DrawerLayout
import org.jetbrains.anko.support.v4.drawerLayout

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


abstract class MateriaActivityUI<Activity> : AnkoComponent<Activity>
{
    override fun createView(ui: AnkoContext<Activity>) = with(ui)
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

                    setNavigationItemSelectedListener()
                    {
                        startActivity<SyncActivity>()
                        true
                    }

                    menu.add("Sync").setOnMenuItemClickListener()
                    {
                        startActivity<SyncActivity>()
                        true
                    }
                    menu.add("Inbox")
                    menu.add("Calendar")
                    menu.add("Settings")
                }.lparams(width = wrapContent, height = matchParent, gravity = Gravity.START)

                fillActivityUI(this)
            }
        }
    }

    abstract fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout)
}