package snakesoft.minion.Models

class WpModel (var Db: LocalDatabase)
{
    var SuccessAttempts: Int = 0
        set(newValue)
        {
            field = newValue
            saveState()
        }

    var FailedAttempts: Int = 0
        set(newValue)
        {
            field = newValue
            saveState()
        }

    init
    {
        loadState()
    }

    private fun loadState()
    {
        try
        {
            val splitted = Db["WillPowerTracking"].split(' ')
            SuccessAttempts = splitted[0].toInt()
            FailedAttempts = splitted[1].toInt()
        }
        catch (ex: Exception)
        {

        }

    }

    private fun saveState()
    {
        Db.put("WillPowerTracking", "$SuccessAttempts $FailedAttempts")
    }
}
