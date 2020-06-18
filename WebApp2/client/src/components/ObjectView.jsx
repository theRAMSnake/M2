import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'
import m3proxy from '../modules/m3proxy'

import ListObjectView from './ListObjectView.jsx'
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import GenericObjectDialog from './dialogs/GenericObjectDialog.jsx'

import DefaultThumbnail from './thumbnails/Default.jsx'
import SimpleThumbnail from './thumbnails/Simple.jsx'
import ListThumbnail from './thumbnails/List.jsx'

import {
    Card,
    CardContent,
    CardHeader,
    Typography,
    Divider,
    IconButton,
    Avatar
} from "@material-ui/core";

import SettingsIcon from '@material-ui/icons/Settings';
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import EditIcon from '@material-ui/icons/Edit';

function getObjectThumbnail(obj)
{
    const type = m3proxy.getType(obj.typename);
    if(type.name === 'simple_list')
    {
        return (<ListThumbnail value={obj}/>);
    }

    if(type.fields && type.fields.length == 1)
    {
        return (<SimpleThumbnail value={obj[type.fields[0].name]}/>);
    }

    return (<DefaultThumbnail value={obj}/>);
}

function ObjectView(props) 
{
    const obj = props.value;

    const [inDeleteDialog, setinDeleteDialog] = useState(false);
    const [inEditDialog, setinEditDialog] = useState(false);
    const [visible, setVisible] = useState(true);
    const [object, setObject] = useState(obj);
    const [objectInEdit, setObjectInEdit] = useState(obj);
    const [changed, setChanged] = useState(false);

    function deleteClicked(e){
        setinDeleteDialog(true);
    }

    function editClicked(e){
        setinEditDialog(true);
        setChanged(false);
        setObjectInEdit(object);
    }

    function onDeleteDialogCancel(e){
        setinDeleteDialog(false);
    }

    function onEditDialogCancel(e){
        setinEditDialog(false);
    }

    function onDeleteDialogOk(e){
        setinDeleteDialog(false);
        setVisible(false);
        MateriaRequest.postDelete(obj.id);
    }

    function onObjectChanged(obj)
    {
        setChanged(true);
        setObjectInEdit(JSON.parse(JSON.stringify(obj)));
    }

    function onObjectChangedAndCommit(obj)
    {
        MateriaRequest.postEdit(obj.id, JSON.stringify(obj));
        setObject(JSON.parse(JSON.stringify(obj)));
    }
    
    function onEditDialogOk(e)
    {
        setinEditDialog(false);

        if(changed)
        {
            setObject(objectInEdit);
            MateriaRequest.postEdit(obj.id, JSON.stringify(objectInEdit));
        }
    }

    function getObjectDialog()
    {
        if(obj.typename === 'simple_list')
        {
            return <ListObjectView open={inEditDialog} object={object} onChange={onObjectChangedAndCommit} onClose={onEditDialogCancel}/>
        }

        return (<GenericObjectDialog open={inEditDialog} onClose={onEditDialogCancel} onChange={onObjectChanged} object={objectInEdit} />);
    }

    return (
        visible && <div>
        <Card style={{ width: '25vw', height: '33vh', margin: '5px'}}>
            <CardHeader 
                avatar={<Avatar><SettingsIcon /></Avatar>}
                title={<Typography variant="body1" color='secondary'>{obj.name ? obj.name : obj.id}</Typography>}
                subheader={obj.type}
                action={
                    <div>
                        <IconButton onClick={editClicked}>
                            <EditIcon/>
                        </IconButton>
                        <IconButton onClick={deleteClicked}>
                            <DeleteForeverIcon/>
                        </IconButton>
                    </div>
                }/>
            <Divider/>
            <CardContent>
                {getObjectThumbnail(object)}
            </CardContent>
        </Card>
        <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
        {getObjectDialog()}
        </div>
    );
}

export default ObjectView;