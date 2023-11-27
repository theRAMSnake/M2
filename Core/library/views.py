import json
import m4
from collection import Collection

def toDict(obj):
   item_dict = {}

   # Dynamically gather the attributes from the item
   for attr in dir(obj):
       x = getattr(obj, attr)
       if isinstance(x, m4.MateriaObject):
           item_dict[attr] = toDict(getattr(obj, attr))
           continue

       if isinstance(x, list):
           item_dict[attr] = []
           for y in x:
               item_dict[attr].append(toDict(y))
           continue

       if not attr.startswith('_') and not callable(x):
           item_dict[attr] = x

   return item_dict

def collection_to_json(name):
    collection = Collection(name)

    items = collection.get_items()

    items_as_dicts = []
    for item in items:
        items_as_dicts.append(toDict(item))

    result = {}
    result["items"] = items_as_dicts
    result["header"] = toDict(collection.header)

    json_str = json.dumps(result, default=str)
    return json_str
