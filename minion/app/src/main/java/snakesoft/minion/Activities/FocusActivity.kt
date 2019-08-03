package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.text.Html
import android.text.Spanned
import android.text.SpannedString
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel
import java.util.*

class FocusActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        FocusActivityUI().setContentView(this)
    }
}

class FocusItemAdapter : BaseAdapter()
{
    override fun getItemId(p0: Int): Long {
        return p0.toLong()
    }

    override fun getView(i : Int, v : View?, parent : ViewGroup?) : View
    {
        val itemId = getItem(i)
        return with(parent!!.context) {
            relativeLayout()
            {
                textView(getItemText(itemId))
                {
                    textSize = 32f
                    textColor = Color.BLUE
                    onClick {
                        GlobalModel.FocusDataModel.toggleItem(itemId)
                        text = getItemText(itemId)
                    }
                }
            }
        }
    }

    fun getItemText(id: UUID): Spanned? {
        val rawText = GlobalModel.FocusDataModel.getItemName(id)
        return if (GlobalModel.FocusDataModel.isItemToggled(id)) Html.fromHtml("<s>$rawText</s>") else SpannedString(rawText)
    }

    override fun getItem(position : Int) : UUID {
        return GlobalModel.FocusDataModel.Items[position].id
    }

    override fun getCount() : Int {
        return GlobalModel.FocusDataModel.Items.size
    }
}

class FocusActivityUI : MateriaActivityUI<FocusActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            relativeLayout()
            {
                listView()
                {
                    adapter = FocusItemAdapter()
                }
            }
        }
    }
}