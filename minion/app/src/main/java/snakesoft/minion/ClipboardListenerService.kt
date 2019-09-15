package snakesoft.minion

import android.app.Service
import android.content.Intent
import android.os.Binder
import android.os.IBinder
import android.content.ClipboardManager
import android.content.Context
import snakesoft.minion.Activities.getInvalidId
import snakesoft.minion.Models.Event1
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.Models.InboxItem

object ClipboardListener : ClipboardManager.OnPrimaryClipChangedListener
{
    var ClipboardManager: ClipboardManager? = null
    val OnNewText = Event1<String>()

    override fun onPrimaryClipChanged()
    {
        OnNewText(ClipboardManager?.primaryClip?.getItemAt(0)?.text.toString())
    }
}

class ClipboardListenerService : Service()
{
    inner class LocalBinder : Binder()

    override fun onBind(p0: Intent?): IBinder
    {
        return LocalBinder()
    }

    override fun onCreate()
    {
        val clipboard = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        ClipboardListener.ClipboardManager = clipboard
        clipboard.addPrimaryClipChangedListener(ClipboardListener)

        ClipboardListener.OnNewText += { s ->
            if(GlobalModel.InboxModel.Items.getAvailableItems().find { it.text == s } == null)
                GlobalModel.InboxModel.Items.add(InboxItem(getInvalidId(), s))
        }
    }
}