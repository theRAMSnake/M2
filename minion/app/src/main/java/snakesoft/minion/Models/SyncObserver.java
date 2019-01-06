package snakesoft.minion.Models;

import java.util.Vector;

class SyncedEntity
{
    public SyncedEntity(String name_)
    {
        name = name_;
    }

    public String name;
    public int changed = 0;
    public int added = 0;
    public int loaded = 0;
    public int deleted = 0;
}

public class SyncObserver
{
    public void beginSync(String name)
    {
        mSyncedEntity = new SyncedEntity(name);
    }

    public void endSync()
    {
        if(mSyncedEntity != null)
        {
            mSyncedEntities.add(mSyncedEntity);
        }
    }

    private Vector<SyncedEntity> mSyncedEntities = new Vector<>();
    private SyncedEntity mSyncedEntity;

    public void itemChanged()
    {
        mSyncedEntity.changed++;
    }

    public void itemDeleted()
    {
        mSyncedEntity.deleted++;
    }

    public void itemAdded()
    {
        mSyncedEntity.added++;
    }

    public void itemLoaded(int itemsCount)
    {
        mSyncedEntity.loaded += itemsCount;
    }

    public String getLog()
    {
        StringBuilder b = new StringBuilder();

        for (SyncedEntity en : mSyncedEntities)
        {
           b.append(en.name);
           b.append(System.lineSeparator());

           b.append("+");
           b.append(en.added);

           b.append("-");
           b.append(en.deleted);

           b.append("=");
           b.append(en.changed);

           b.append("L");
           b.append(en.loaded);

           b.append(System.lineSeparator());
           b.append(System.lineSeparator());
        }

        return b.toString();
    }
}
