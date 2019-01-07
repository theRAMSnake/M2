package snakesoft.minion.Activities;

/**
 * Created by snake on 4/28/17.
 */

public interface SyncListener
{
    void onSyncComplete();

    void onSyncError();
}