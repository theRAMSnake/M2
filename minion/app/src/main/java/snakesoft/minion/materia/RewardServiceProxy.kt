package snakesoft.minion.materia

@ExperimentalUnsignedTypes
class RewardServiceProxy(private val mMateriaConnection: MateriaConnection)
{
    @Throws(MateriaUnreachableException::class)
    fun addPoints(pts: UInt)
    {
        val jsonData = "{\"operation\": \"reward\", \"points\": $pts}"
        mMateriaConnection.sendMessage(jsonData)
    }
}
