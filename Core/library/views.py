import json
from collection import Collection

def collection_to_json(name):
    collection = Collection(name)

    items = collection.get_items()

    items_as_dicts = []
    for item in items:
        item_dict = {}

        # Dynamically gather the attributes from the item
        for attr in dir(item):
            if not callable(getattr(item, attr)):
                item_dict[attr] = getattr(item, attr)

        items_as_dicts.append(item_dict)

    json_str = json.dumps(items_as_dicts, default=str)
    return json_str
