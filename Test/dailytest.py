import collection
events = collection.Collection("events").get_items()
if len(events) == 1 and events[0].result == "Core daily update finished successfuly":
    result = 1
elif len(events) != 0:
    result = events[0].result
else:
    result = 0
