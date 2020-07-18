package snakesoft.minion.Activities

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.text.Html
import android.view.View
import com.unnamed.b.atv.model.TreeNode
import org.jetbrains.anko.support.v4._DrawerLayout
import com.unnamed.b.atv.view.AndroidTreeView
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.JournalIndexItem
import java.util.*


class NewsActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        NewsActivityUI().setContentView(this)
    }
}

class NewsActivityUI : MateriaActivityUI<NewsActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            linearLayout {
                webView {
                    loadDataWithBaseURL(null, GlobalModel.NewsModel.get(), "text/html", null, null)
                }
            }
        }
    }
}