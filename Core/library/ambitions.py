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

def get_failed_ambition_colors():
    reward_cb_object = common.get_or_create('reward.cb')

    if reward_cb_object and hasattr(reward_cb_object, 'failed_colors'):
        failed_colors_str = reward_cb_object.failed_colors
        return json.loads(failed_colors_str)
    else:
        # If the object or field doesn't exist, return an empty list.
        return []

def save_failed_ambition_colors(failed_colors):
    failed_colors_str = json.dumps(failed_colors)
    reward_cb_object = common.get_or_create('reward.cb')
    reward_cb_object.failed_colors = failed_colors_str
    m4.modify('reward.cb', reward_cb_object)

def update(today):
    ambitions_collection = Collection('ambitions')
    ambitions = ambitions_collection.get_items()

    failed_colors = get_failed_ambition_colors()

    # If there are fewer than 3 active ambitions, we'll penalize by draining two coins.
    if len(ambitions) < 3:
        reward.reward(-2)

    # Now, we'll go through each ambition and check its status.
    for amb in ambitions:
        if str_to_date(amb.expiry) <= today and not (hasattr(amb, 'completed') and bool(amb.completed)):
            failed_colors.append(amb.color)
            save_failed_ambition_colors(failed_colors)
            ambitions_collection.remove(amb.id)
            continue

        elif hasattr(amb, 'completed') and bool(amb.completed):
            if not hasattr(amb, 'bonus_end_date'):
                amb.bonus_end_date = str(str_to_date(amb.expiry) + timedelta(days=random.randint(10, 60)))
                while amb.color in failed_colors:
                    failed_colors.remove(amb.color)
                    save_failed_ambition_colors(failed_colors)
                m4.modify(amb.id, amb)

            if today <= str_to_date(amb.bonus_end_date):
                # If we're within the bonus period, grant the rewards daily.
                reward.reward(2, amb.color)  # The reward is in the ambition's color.
            else:
                # If the bonus period has ended, delete the ambition from the collection.
                ambitions_collection.remove(amb.id)

    # Apply a daily penalty for each color that has failed ambitions until replaced.
    for color in failed_colors:
        reward.reward(-2, color)  # Draining two coins as a penalty.

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
