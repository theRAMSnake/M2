package snakesoft.minion.materia

import calendar.Calendar
import common.Common
import finance.Finance
import inbox.Inbox
import snakesoft.minion.Models.InboxItem
import snakesoft.minion.Models.FinanceEvent
import strategy.Strategy

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

fun toProto(x: FinanceEvent) : Finance.EventInfo
{
    val b = Finance.EventInfo.newBuilder()

    b.eventId = toProto(x.eventId)
    b.categoryId = toProto(x.categoryId)
    b.amountEuroCents = x.amountCents.toInt()
    b.timestamp = x.timestamp.toInt()
    b.details = x.details

    return b.build()
}