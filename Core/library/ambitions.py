from datetime import datetime
from datetime import timedelta
import random
import m4
from collection import Collection
import reward
import common
from common import str_to_date
import json

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

def update(today):
    ambitions_collection = Collection('ambitions')
    ambitions = ambitions_collection.get_items()

    # If there are fewer than 3 active ambitions, we'll penalize by draining two coins.
    if len(ambitions) < 3:
        reward.reward(-2)

    # Now, we'll go through each ambition and check its status.
    for amb in ambitions:
        if hasattr(amb, 'completed') and bool(amb.completed):
            if not hasattr(amb, 'bonus_end_date'):
                amb.bonus_end_date = str(today + timedelta(days=random.randint(20, 60)))
                m4.modify(amb.id, amb)

            if today <= str_to_date(amb.bonus_end_date):
                # If we're within the bonus period, grant the rewards daily.
                reward.reward(2, amb.color)  # The reward is in the ambition's color.
            else:
                # If the bonus period has ended, delete the ambition from the collection.
                ambitions_collection.remove(amb.id)
        elif str_to_date(amb.expiry) <= today:
            reward.reward(-2, amb.color)

def complete_ambition(ambition_id):
    # Access the collection of ambitions.
    ambitions_collection = Collection('ambitions')

    # Retrieve all ambitions and find the one with the provided ID.
    all_ambitions = ambitions_collection.get_items()
    ambition_to_complete = next((amb for amb in all_ambitions if amb.id == ambition_id), None)

    if ambition_to_complete is None:
        raise ValueError("Ambition not found")

    if hasattr(ambition_to_complete, 'completed') and bool(ambition_to_complete.completed):
        raise ValueError("Ambition already completed")

    # Mark the ambition as completed and set the completion date.
    ambition_to_complete.completed = True

    # Save the updated ambition back to the collection.
    m4.modify(ambition_id, ambition_to_complete)
