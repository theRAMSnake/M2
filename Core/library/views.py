import json
import m4
from collection import Collection

def toDict(obj):
   item_dict = {}

   # Dynamically gather the attributes from the item
   for attr in dir(obj):
       if isinstance(getattr(obj, attr), m4.MateriaObject):
           item_dict[attr] = toDict(getattr(obj, attr))
           continue

       if not attr.startswith('_') and not callable(getattr(obj, attr)):
           item_dict[attr] = getattr(obj, attr)

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
