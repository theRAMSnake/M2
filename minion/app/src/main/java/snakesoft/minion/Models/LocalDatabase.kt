package snakesoft.minion.Models

import android.content.Context

import com.google.common.io.Files

import java.io.BufferedReader
import java.io.File
import java.io.FileInputStream
import java.io.FileNotFoundException
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream
import java.io.InputStreamReader
import java.io.RandomAccessFile

/**
 * Created by snake on 4/30/17.
 */

class LocalDatabase(private val mContext: Context) {

    operator fun get(path: String): ByteArray? {
        try {
            val f = RandomAccessFile(mContext.applicationInfo.dataDir + "/" + path, "r")
            val b = ByteArray(f.length().toInt())
            f.readFully(b)
            return b
        } catch (ex: FileNotFoundException) {

        } catch (ex: IOException) {

        }

        return null
    }

    fun put(path: String, data: ByteArray) {
        try {
            Files.write(data, File(mContext.applicationInfo.dataDir + "/" + path))
        } catch (e: IOException) {

        }

    }
}
