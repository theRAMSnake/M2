package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import java.util.*

class ResourcesActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        ResourcesActivityUI().setContentView(this)
    }
}

class ResourceItemAdapter : BaseAdapter()
{
    override fun getItemId(p0: Int): Long {
        return p0.toLong()
    }

    override fun getView(i : Int, v : View?, parent : ViewGroup?) : View
    {
        val item = getItem(i)
        return with(parent!!.context) {
            relativeLayout()
            {
                textView(GlobalModel.StrategyModel.getItemName(item))
                {
                    textSize = 64f
                    textColor = Color.BLUE
                    onClick {
                        GlobalModel.StrategyModel.decItem(item)
                        notifyDataSetChanged()
                    }
                }.lparams()
                {
                    leftMargin = dip(15)
                    alignParentLeft()
                }
                textView(GlobalModel.StrategyModel.getItemValue(item).toString())
                {
                    textSize = 64f
                    textColor = Color.BLACK
                    onClick {
                        GlobalModel.StrategyModel.incItem(item)
                        notifyDataSetChanged()
                    }
                }.lparams()
                {
                    rightMargin = dip(30)
                    alignParentRight()
                }
            }
        }
    }

    override fun getItem(position : Int) : UUID {
        return GlobalModel.StrategyModel.Ids[position]
    }

    override fun getCount() : Int {
        return GlobalModel.StrategyModel.Ids.size
    }
}

class ResourcesActivityUI : MateriaActivityUI<ResourcesActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            relativeLayout()
            {
                listView()
                {
                    adapter = ResourceItemAdapter()
                }
            }
        }
    }
}