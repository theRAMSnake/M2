package snakesoft.minion.Models

import android.content.Context

import com.google.common.io.Files

import android.R.attr.data
import org.zeromq.ZMQ.context
import java.io.*
import org.zeromq.ZMQ.context




/**
 * Created by snake on 4/30/17.
 */

class LocalDatabase(private val mContext: Context) {

    operator fun get(path: String): String
    {
        var ret = ""

        try
        {
            val inputStream = mContext.openFileInput(mContext.applicationInfo.dataDir + "/" + path)
            ret =  inputStream.bufferedReader().use { it.readText() }
        }
        catch (e: FileNotFoundException) {

        } catch (e: IOException) {

        }

        return ret
    }

    fun put(path: String, data: String)
    {
        try
        {
            val outputStreamWriter = OutputStreamWriter(mContext.openFileOutput(mContext.applicationInfo.dataDir + "/" + path, Context.MODE_PRIVATE))
            outputStreamWriter.write(data)
            outputStreamWriter.close()
        }
        catch (e: IOException)
        {

        }

    }
}
