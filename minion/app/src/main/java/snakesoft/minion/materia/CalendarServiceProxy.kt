package snakesoft.minion.materia

import com.google.protobuf.InvalidProtocolBufferException

import calendar.Calendar
import common.Common

class CalendarServiceProxy(private val mMateriaConnection: MateriaConnection) {

    @Throws(InvalidProtocolBufferException::class, MateriaUnreachableException::class)
    fun query(input: Common.TimeRange): Calendar.CalendarItems {
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
