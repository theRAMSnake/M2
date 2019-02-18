package snakesoft.minion.Models

class Event
{
    private val handlers = arrayListOf<(Event.() -> Unit)>()
    operator fun plusAssign(handler: Event.() -> Unit) { handlers.add(handler) }
    operator fun invoke() { for (handler in handlers) handler() }
}