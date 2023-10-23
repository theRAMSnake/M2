from datetime import datetime
import collection
import m4
import ambitions
import reward

def m4_update(today):
    current_day = datetime.now().weekday()

    # Check if the current day is a weekday
    if 0 <= current_day <= 4:  # 0 is Monday, 4 is Friday
        reward.reward(-10)
    ambitions.update(today)

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

