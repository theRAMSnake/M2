package snakesoft.minion.Materia;

import com.google.protobuf.InvalidProtocolBufferException;

import calendar.Calendar;
import common.Common;

/**
 * Created by snake on 11/24/17.
 */

/*
   rpc Query (TimeRange) returns (CalendarItems);
   rpc Next (NextQueryParameters) returns (CalendarItems);
   rpc DeleteItem (common.UniqueId) returns (common.OperationResultMessage);
   rpc EditItem (CalendarItem) returns (common.OperationResultMessage);
   rpc AddItem (CalendarItem) returns (common.UniqueId);
*/

public class CalendarServiceProxy
{
    public CalendarServiceProxy(MateriaConnection materiaConnection)
    {
        mMateriaConnection = materiaConnection;
    }

    public Calendar.CalendarItems query(Calendar.TimeRange input) throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Calendar.CalendarItems.parseFrom(mMateriaConnection.sendMessage(
                input.toByteString(),
                "CalendarService",
                "Query"
        ));}

    public Calendar.CalendarItems next(Calendar.NextQueryParameters input) throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Calendar.CalendarItems.parseFrom(mMateriaConnection.sendMessage(
                input.toByteString(),
                "CalendarService",
                "Next"
        ));}

    public void deleteItem(Common.UniqueId id) throws InvalidProtocolBufferException, MateriaUnreachableException {
        mMateriaConnection.sendMessage(
                id.toByteString(),
                "CalendarService",
                "DeleteItem"
        );
    }

    public Common.UniqueId addItem(Calendar.CalendarItem item) throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "CalendarService",
                "AddItem"
        ));
    }

    public boolean editItem(Calendar.CalendarItem item) throws InvalidProtocolBufferException, MateriaUnreachableException {
        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "CalendarService",
                "EditItem"
        )).getSuccess();
    }

    private MateriaConnection mMateriaConnection;
}
