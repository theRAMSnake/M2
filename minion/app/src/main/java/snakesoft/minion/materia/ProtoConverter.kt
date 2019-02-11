package snakesoft.minion.materia

import calendar.Calendar
import common.Common
import inbox.Inbox
import snakesoft.minion.Models.CalendarItem
import snakesoft.minion.Models.InboxItem

fun toProto(x: java.util.UUID) : Common.UniqueId
{
    return Common.UniqueId.newBuilder().setGuid(x.toString()).build()
}

fun toProto(x: CalendarItem) : Calendar.CalendarItem
{
    val b = Calendar.CalendarItem.newBuilder()

    b.id = toProto(x.id)
    b.text = x.text
    b.timestamp = x.timestamp

    return b.build()
}

fun toProto(x: InboxItem) : Inbox.InboxItemInfo
{
    val b = Inbox.InboxItemInfo.newBuilder()

    b.id = toProto(x.id)
    b.text = x.text

    return b.build()
}