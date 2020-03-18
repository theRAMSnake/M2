package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel

class RewardActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)

        RewardActivityUI().setContentView(this)
    }
}

class RewardActivityUI : MateriaActivityUI<RewardActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            verticalLayout{
                val t = textView(GlobalModel.RewardModel.Points.toString())
                button("Add") {
                    onClick {
                        GlobalModel.RewardModel.Points++
                        t.text = GlobalModel.RewardModel.Points.toString()
                    }
                }
            }
        }
    }
}