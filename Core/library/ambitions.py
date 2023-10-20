from datetime import datetime
import m4
from collection import Collection
import reward

def create_ambition(name, color, expiry):
    # Validation checks throw an exception if there are issues.
    if not name:
        raise ValueError("Name cannot be empty.")

    valid_colors = reward.get_reward_colors()
    if color not in valid_colors:
        raise ValueError(f"Invalid color. Must be one of {valid_colors}.")

    if not isinstance(expiry, datetime):
        raise ValueError("Expiry must be a datetime object.")

    today = datetime.now()
    if expiry <= today:
        raise ValueError("Expiry must be in the future.")

    ambitions_collection = Collection('ambitions')

    existing_ambitions = ambitions_collection.get_items()

    if len(existing_ambitions) >= 3:
        raise ValueError("Cannot have more than 3 ambitions.")

    # Check for an ambition with the same color.
    for ambition in existing_ambitions:
        if ambition.color == color:
            raise ValueError("An ambition with the same color already exists.")

    # Create a new ambition instance as a MateriaObject.
    new_ambition = m4.MateriaObject()
    new_ambition.name = name
    new_ambition.color = color
    new_ambition.expiry = expiry

    # Add the new ambition to the collection
    ambitions_collection.add(new_ambition)
