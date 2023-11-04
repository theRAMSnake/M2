import m4

def create_collection(name):
    """
    Create a new collection with a specified name. This collection will be a child
    of a special '.collections' object.

    :param name: The name of the new collection.
    :return: A Collection object representing the new collection.
    """
    COLLECTIONS_PARENT_ID = '.collections'  # The ID of the parent object for collections.
    COLLECTION_TYPE = 'object'  # All entities, including collections, are of type 'object'.

    # Check if the '.collections' object exists using query_ids function.
    collections_exist = m4.query_ids([COLLECTIONS_PARENT_ID])

    if len(collections_exist) == 0:
        # If the '.collections' object doesn't exist, we create it here.
        collections_header = m4.MateriaObject()
        m4.create(COLLECTIONS_PARENT_ID, COLLECTION_TYPE, collections_header)

    # Expression to find the collection object by name under the collections header.
    expression = f'ChildOf("{COLLECTIONS_PARENT_ID}") AND .name = "{name}"'
    existing_collections = m4.query_expr(expression)

    if len(existing_collections) > 0:
        # If a collection with this name already exists, we cannot create another.
        raise Exception(f"A collection with the name '{collection_name}' already exists.")

    # Prepare the attributes for the new collection object.
    collection_data = m4.MateriaObject()
    collection_data.name = name

    new_collection_id = m4.create(name, COLLECTION_TYPE, collection_data, COLLECTIONS_PARENT_ID)

    if not new_collection_id:
        raise Exception("Failed to create the new collection.")

    return new_collection_id

def remove_collection(collection_name):
    """
    Remove a collection by its name.

    :param collection_name: The name of the collection to be removed.
    :return: None
    """

    if not isinstance(collection_name, str):
        raise ValueError("The collection name must be a string.")

    # First, we need to identify the ID of the collection using its name.
    # This assumes that there's a way to query collections by their name.

    # Querying for the collections header object
    collections_header = m4.query_ids([".collections"])

    if len(collections_header) == 0:
        raise Exception("Collections header object does not exist.")

    # The expression to find the collection object by name under the collections header.
    expression = f'ChildOf("{collections_header[0].id}") AND .name = "{collection_name}"'
    collections = m4.query_expr(expression)

    if len(collections) == 0:
        raise ValueError(f"No collection exists with the name: {collection_name}")

    # Assuming that each collection name is unique and 'query_expr' returns a list of collections,
    # we will take the first one.
    collection_to_remove = collections[0].id

    # 'erase' is used for object deletion in your 'm4' system.
    m4.erase(collection_to_remove)

class Collection:
    def __init__(self, collection_name):
        self.collection_name = collection_name
        self.header_id = self._get_header_id(collection_name)

    def _get_header_id(self, collection_name):
        # Query for the collection header by name
        collections = m4.query_expr(f'ChildOf(".collections") AND .name = "{collection_name}"')

        if len(collections) == 0:
            create_collection(collection_name)
            collections = m4.query_expr(f'ChildOf(".collections") AND .name = "{collection_name}"')

        # Assuming the ID of the collection is the first element queried.
        return collections[0].id

    def add(self, item):
        return m4.create("", "object", item, self.header_id)

    def remove(self, object_id):
        """Remove an object from the collection."""
        m4.erase(object_id)

    def get_items(self):
        """Fetch and return all items from the collection."""
        return m4.query_expr(f'ChildOf("{self.header_id}")')

