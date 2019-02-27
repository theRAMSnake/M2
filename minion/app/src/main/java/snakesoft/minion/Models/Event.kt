package snakesoft.minion.Models

class Event
{
    private val handlers = arrayListOf<(Event.() -> Unit)>()
    operator fun plusAssign(handler: Event.() -> Unit) { handlers.add(handler) }
    operator fun invoke() { for (handler in handlers) handler() }
}

class Event1<T>
{
    private val handlers = arrayListOf<(Event1<T>.(t: T) -> Unit)>()
    operator fun plusAssign(handler: Event1<T>.(t: T) -> Unit) { handlers.add(handler) }
    operator fun invoke(t: T) { for (handler in handlers) handler(t) }
}