package snakesoft.minion.Models

interface SyncListener
{
    fun onSyncComplete()
    fun onSyncError()
}
