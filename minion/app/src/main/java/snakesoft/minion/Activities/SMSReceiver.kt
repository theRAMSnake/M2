package snakesoft.minion.Activities

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.util.Log

object MyIntentFilter : IntentFilter()
{
    init {
        priority = 1000
        addAction("android.provider.Telephony.SMS_RECEIVED")
    }
}

object SMSReceiver : BroadcastReceiver() {

    override fun onReceive(context: Context, intent: Intent) {
        Log.println(Log.DEBUG, "sms received", "sms received")
    }
}