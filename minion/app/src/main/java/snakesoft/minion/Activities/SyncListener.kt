package snakesoft.minion.Activities

/**
 * Created by snake on 4/28/17.
 */

interface SyncListener {
    fun onSyncComplete()

    fun onSyncError()
}