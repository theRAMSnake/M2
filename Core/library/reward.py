import m4
import random
import collection

def get_reward_colors():
    return ["Red", "Blue", "Yellow", "Green", "Purple"]

def reward(number, color=None):
    # First, we need to retrieve the 'reward.coins' object from Materia
    try:
        coins_object = m4.query_ids(['reward.coins'])[0]
    except Exception as e:
        initial = m4.MateriaObject()
        initial.Red = 0
        initial.Blue = 0
        initial.Yellow = 0
        initial.Green = 0
        initial.Purple = 0
        initial.Gold = 0
        m4.create('reward.coins', 'object', initial)
        coins_object = m4.query_ids(['reward.coins'])[0]

    # Get the current state of coins from the object's attributes
    coins = {coin_color: int(getattr(coins_object, coin_color, 0)) for coin_color in get_reward_colors()}
    if hasattr(coins_object, "Gold"):
        coins["Gold"] = int(coins_object.Gold)
    else:
        coins["Gold"] = 0

    if color:
        # If a specific color is specified, we adjust the number of coins of that color
        if color not in coins:
            raise ValueError(f"Invalid color: {color}. Must be one of {get_reward_colors()}.")
        coins[color] += number  # This assumes 'number' could be negative for subtraction
        if coins[color] < 0:
            coins[color] = 0
    else:
        # If no color is specified, we randomly select a color for each coin change
        for _ in range(abs(number)):  # Using absolute value to accommodate subtraction
            if number > 0:
                random_color = random.choice(get_reward_colors())
                coins[random_color] += 1
            else:
                available_colors = [c for c, count in coins.items() if count > 0]

                if not available_colors:
                    break

                color = random.choice(available_colors)
                coins[color] -= 1

    objs = m4.query_ids(["discipline.level"])
    if len(objs) > 0 and number > 0:
        val = int(objs[0].value)
        chance = 1 - 100.0 / float(val + 100);
        for _ in range(number):
            if random.random() <= chance:
                coins["Gold"] += 1

    # Now, we'll update the 'reward.coins' object with the new values
    for coin_color, value in coins.items():
        setattr(coins_object, coin_color, str(value))

    m4.modify(coins_object.id, coins_object)
