package snakesoft.minion.Models

import android.content.Context
import android.os.Environment
import java.io.*
import android.os.Environment.getExternalStorageDirectory



class LocalDatabase
{
    operator fun get(path: String): String
    {
        val fullpath = Environment.getExternalStorageDirectory().absolutePath + "/" + path
        return File(fullpath).readText()
    }

    fun put(path: String, data: String)
    {
        try
        {
            val fullpath = Environment.getExternalStorageDirectory().absolutePath + "/" + path
            File(fullpath).writeText(data)
        }
        catch (e: IOException)
        {

        }

    }

    fun contains(path: String): Boolean
    {
        val fullpath = Environment.getExternalStorageDirectory().absolutePath + "/" + path
        return File(fullpath).exists()
    }
}
