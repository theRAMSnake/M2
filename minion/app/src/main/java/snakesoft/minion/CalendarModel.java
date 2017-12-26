package snakesoft.minion;

import android.content.ContentResolver;
import android.database.Cursor;
import android.provider.CalendarContract;

import com.google.protobuf.InvalidProtocolBufferException;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Vector;

import actions.Actions;
import calendar.Calendar;

/**
 * Created by snake on 11/24/17.
 */

public class CalendarModel
{
    public CalendarModel(CalendarServiceProxy proxy)
    {
        mProxy = proxy;
        mItemsChanges = new Vector<>();
    }

    public void sync() throws MateriaUnreachableException
    {
        try
        {
            for(int i = 0; i < mItemsChanges.size(); ++i)
            {
                if(mItemsChanges.get(i).type == StatusOfChange.Type.Edit)
                {
                    mProxy.editItem(mItems.getItems(i));
                }
                else if(mItemsChanges.get(i).type == StatusOfChange.Type.Delete)
                {
                    mProxy.deleteItem(mItems.getItems(i).getId());
                }
                else if(mItemsChanges.get(i).type == StatusOfChange.Type.Add)
                {
                    mProxy.addItem(mItems.getItems(i));
                }
            }

            mItemsChanges.clear();

            mItems = queryAllItems();
            for(int i = 0; i < mItems.getItemsCount(); ++i)
            {
                mItemsChanges.addElement(new StatusOfChange());
            }
        }
        catch (InvalidProtocolBufferException ex)
        {

        }

        saveState();
    }

    public void resetChanges()
    {
        mItemsChanges.clear();
    }

    /* Returns values in range [timestampFrom, timestampTo) */
    public List<Calendar.CalendarItem> getItems(long timestampFrom, long timestampTo)
    {
        System.out.print(timestampFrom);
        System.out.print(timestampTo);
        List<Calendar.CalendarItem> result = new Vector<>();

        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            Calendar.CalendarItem curItem = mItems.getItems(i);
            if(curItem.getTimestamp() >= timestampFrom && curItem.getTimestamp() < timestampTo)
            {
                if(mItemsChanges.get(i).type != StatusOfChange.Type.Delete &&
                        mItemsChanges.get(i).type != StatusOfChange.Type.Junk)
                {
                    result.add(curItem);
                }
            }
        }

        return result;
    }

    public List<Calendar.CalendarItem> getAllItems()
    {
        return new ArrayList<Calendar.CalendarItem>(mItems.getItemsList());
    }

    public void modifyItem(Calendar.CalendarItem item)
    {
        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            if(mItems.getItems(i).getId().getGuid().equals(item.getId().getGuid()))
            {
                mItems = Calendar.CalendarItems.newBuilder(mItems).setItems(i, item).build();

                if(mItemsChanges.get(i).type != StatusOfChange.Type.Add)
                {
                    StatusOfChange ch = new StatusOfChange();
                    ch.type = StatusOfChange.Type.Edit;
                    mItemsChanges.set(i, ch);
                }

                break;
            }
        }

        saveState();
    }

    public void deleteItem(String guid)
    {
        for(int i = 0; i < mItems.getItemsCount(); ++i)
        {
            if (mItems.getItems(i).getId().getGuid().equals(guid))
            {
                StatusOfChange newStatus = new StatusOfChange();

                newStatus.type = mItemsChanges.get(i).type == StatusOfChange.Type.Add
                        ? StatusOfChange.Type.Junk
                        : StatusOfChange.Type.Delete;

                mItemsChanges.set(i, newStatus);
                break;
            }
        }

        saveState();
    }

    public void addItem(Calendar.CalendarItem item)
    {
        mItems = Calendar.CalendarItems.newBuilder(mItems).addItems(item).build();

        StatusOfChange ch = new StatusOfChange();
        ch.type = StatusOfChange.Type.Add;
        mItemsChanges.add(ch);

        saveState();
    }

    public void loadState(LocalDatabase localDb)
    {
        mLocalDb = localDb;
        try
        {
            mItems = Calendar.CalendarItems.parseFrom(localDb.get("CalendarItems"));
            ByteArrayInputStream byteStream = new ByteArrayInputStream(localDb.get("CalendarItemsStatus"));
            int next = byteStream.read();
            while(next != -1)
            {
                StatusOfChange ch = new StatusOfChange();
                ch.type = StatusOfChange.Type.values()[ next];
                mItemsChanges.add(ch);

                next = byteStream.read();
            }

            assert mItemsChanges.size() == mItems.getItemsCount();

            for(int i = 0; i < mItems.getItemsCount(); ++i)
            {
                if(mItems.getItems(i).getId().getGuid().length() < 10)
                {
                    int curVirtualId = Integer.parseInt(mItems.getItems(i).getId().getGuid());
                    if(curVirtualId > mLastVirtualId)
                    {
                        mLastVirtualId = curVirtualId;
                    }
                }
            }
        }
        catch (InvalidProtocolBufferException ex)
        {

        }
        catch (NullPointerException ex)
        {
            if(mItems == null)
            {
                mItems = Calendar.CalendarItems.newBuilder().build();
            }

            //no data in db case
            if(mItems.getItemsCount() != mItemsChanges.size())
            {
                for(int i = 0; i < mItems.getItemsCount(); ++i)
                {
                    mItemsChanges.addElement(new StatusOfChange());
                }
            }
        }
    }

    public void saveState()
    {
        mLocalDb.put("CalendarItems", mItems.toByteArray());

        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        for(StatusOfChange x : mItemsChanges)
        {
            bos.write(x.type.ordinal());
        }

        mLocalDb.put("CalendarItemsStatus", bos.toByteArray());
    }

    public String getNewId()
    {
        return Integer.toString(++mLastVirtualId);
    }

    private Calendar.CalendarItems queryAllItems() throws MateriaUnreachableException
    {
        final long threeYears = 94670778;

        try
        {
            return mProxy.query(Calendar.TimeRange.newBuilder().
                    setTimestampFrom(System.currentTimeMillis() / 1000 - threeYears).
                    setTimestampTo(System.currentTimeMillis() / 1000 + threeYears).build());
        } catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return Calendar.CalendarItems.newBuilder().build();
        }
    }

    private int mLastVirtualId = 0;
    private CalendarServiceProxy mProxy;
    private Calendar.CalendarItems mItems;
    private LocalDatabase mLocalDb;
    private Vector<StatusOfChange> mItemsChanges;
}
