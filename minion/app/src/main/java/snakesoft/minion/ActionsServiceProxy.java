package snakesoft.minion;

/**
 * Created by snake on 5/14/17.
 */

import com.google.protobuf.InvalidProtocolBufferException;

import actions.Actions;
import common.Common;

/**
 rpc GetChildren (common.UniqueId) returns (ActionsList);
 rpc GetParentlessElements (common.EmptyMessage) returns (ActionsList);

 rpc AddElement (ActionInfo) returns (common.UniqueId);
 rpc DeleteElement (common.UniqueId) returns (common.OperationResultMessage);
 rpc EditElement (ActionInfo) returns (common.OperationResultMessage);
 */

public class ActionsServiceProxy
{
    public ActionsServiceProxy(MateriaConnection materiaConnection)
    {
        mMateriaConnection = materiaConnection;
    }

    public Actions.ActionsList getChildren(Common.UniqueId id) throws InvalidProtocolBufferException
    {
        return Actions.ActionsList.parseFrom(mMateriaConnection.sendMessage(
                id.toByteString(),
                "ActionsService",
                "GetChildren"
        ));
    }

    public Actions.ActionsList getParentlessElements() throws InvalidProtocolBufferException
    {
        return Actions.ActionsList.parseFrom(mMateriaConnection.sendMessage(
                Common.EmptyMessage.newBuilder().build().toByteString(),
                "ActionsService",
                "GetParentlessElements"
        ));
    }

    public Common.UniqueId addElement(Actions.ActionInfo nfo) throws InvalidProtocolBufferException
    {
        return Common.UniqueId.parseFrom(mMateriaConnection.sendMessage(
                nfo.toByteString(),
                "ActionsService",
                "AddElement"
        ));
    }

    public void deleteElement(Common.UniqueId id) throws InvalidProtocolBufferException
    {
        mMateriaConnection.sendMessage(
                id.toByteString(),
                "ActionsService",
                "DeleteElement"
        );
    }

    public void editElement(Actions.ActionInfo nfo) throws InvalidProtocolBufferException
    {
        mMateriaConnection.sendMessage(
                nfo.toByteString(),
                "ActionsService",
                "EditElement"
        );
    }

    private MateriaConnection mMateriaConnection;
}
