package snakesoft.minion.Activities

import android.content.Context
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import android.text.InputType
import org.jetbrains.anko.*
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4._DrawerLayout
import snakesoft.minion.Models.GlobalModel

class SyncPrepareActivity : AppCompatActivity()
{
    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)

        SyncPrepareActivityUI().setContentView(this)
    }
}

class SyncPrepareActivityUI() : MateriaActivityUI<SyncPrepareActivity>()
{
    override fun fillActivityUI(_DrawerLayout: @AnkoViewDslMarker _DrawerLayout, ctx: Context)
    {
        with(_DrawerLayout)
        {
            button("Start") {
                onClick {
                    with(ctx)
                    {
                        alert {
                            customView {
                                val t = editText()
                                t.inputType = InputType.TYPE_CLASS_TEXT or InputType.TYPE_TEXT_VARIATION_PASSWORD
                                okButton()
                                {
                                    ctx.startActivity<SyncActivity>("id" to 5588,
                                        "password" to t.text.toString())
                                }
                            }
                        }.show()
                    }
                }
            }
        }
    }
}