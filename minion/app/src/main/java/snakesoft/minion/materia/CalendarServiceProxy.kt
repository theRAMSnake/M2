package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import calendar.Calendar
import common.Common

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

class CalendarServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun query(input: Calendar.TimeRange): Calendar.CalendarItems {
        return Calendar.CalendarItems.parseFrom(mMateriaConnection.sendMessage(
                input.toByteString(),
                "CalendarService",
                "Query"
        ))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun next(input: Calendar.NextQueryParameters): Calendar.CalendarItems {
        return Calendar.CalendarItems.parseFrom(mMateriaConnection.sendMessage(
                input.toByteString(),
                "CalendarService",
                "Next"
        ))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun deleteItem(id: Common.UniqueId) {
        mMateriaConnection.sendMessage(
                id.toByteString(),
                "CalendarService",
                "DeleteItem"
        )
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun addItem(item: Calendar.CalendarItem): Common.UniqueId {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "CalendarService",
                "AddItem"
        ))
    }

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun editItem(item: Calendar.CalendarItem): Boolean {
        return Common.OperationResultMessage.parseFrom(mMateriaConnection.sendMessage(
                item.toByteString(),
                "CalendarService",
                "EditItem"
        )).success
    }
}
