package snakesoft.minion.Models

import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream

/**
 * Created by snake on 6/10/18.
 */

class WpModel {

    var successAtempts: Int
        get() = mSuccessAtempts
        set(newValue) {
            mSuccessAtempts = newValue
            saveState()
        }

    var failedAtempts: Int
        get() = mFailedAtempts
        set(newValue) {
            mFailedAtempts = newValue
            saveState()
        }

    private var mLocalDb: LocalDatabase? = null
    private var mSuccessAtempts = 0
    private var mFailedAtempts = 0
    fun loadState(localDb: LocalDatabase) {
        mLocalDb = localDb
        try {
            val byteStream = ByteArrayInputStream(localDb.get("WillPowerTracking"))
            mSuccessAtempts = byteStream.read()
            mFailedAtempts = byteStream.read()
        } catch (ex: Exception) {
            mSuccessAtempts = 0
            mFailedAtempts = 0
        }

    }

    fun saveState() {
        val bos = ByteArrayOutputStream()

        bos.write(mSuccessAtempts)
        bos.write(mFailedAtempts)

        mLocalDb!!.put("WillPowerTracking", bos.toByteArray())
    }
}
