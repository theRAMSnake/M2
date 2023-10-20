from datetime import datetime
import collection
import m4

def m4_update(today):
    pass

def daily_update():
    events = collection.Collection("events")
    try:
        m4_update(datetime.now())
        ev = m4.MateriaObject()
        ev.result = "Core daily update finished successfuly"
        events.add(ev)
    except Exception as e:
        ev = m4.MateriaObject()
        ev.result = str(e)
        events.add(ev)

