package snakesoft.minion.Models;

public class StatusOfChange implements java.io.Serializable
{
    enum Type
    {
        None,
        Add,
        Delete,
        Edit,
        Junk
    }

    public Type type;

    public StatusOfChange()
    {
        type = Type.None;
    }
}
