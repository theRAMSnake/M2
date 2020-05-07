import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'
import m3proxy from '../modules/m3proxy'
import ObjectProperties from './ObjectProperties.jsx'

import DefaultThumbnail from './thumbnails/Default.jsx'
import SimpleThumbnail from './thumbnails/Simple.jsx'

import {
    Card,
    CardContent,
    CardHeader,
    Typography,
    Divider,
    IconButton,
    Avatar,
    Button,
    Dialog,
    DialogTitle,
    DialogContent,
    DialogContentText,
    DialogActions
} from "@material-ui/core";

import SettingsIcon from '@material-ui/icons/Settings';
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import EditIcon from '@material-ui/icons/Edit';

function getObjectThumbnail(obj)
{
    if(obj.traits.length == 1)
    {
        const trait = m3proxy.getTrait(obj.traits[0]);
        if(trait.requires && trait.requires.length == 1)
        {
            return (<SimpleThumbnail value={obj[trait.requires[0].field]}/>);
        }
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

    function deleteClicked(e){
        setinDeleteDialog(true);
    }

    function editClicked(e){
        setinEditDialog(true);
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
        setObject(obj);
    }

    function onEditDialogOk(e){
        setinEditDialog(false);
        MateriaRequest.postEdit(obj.id, JSON.stringify(object));
    }

    return (
        visible && <div>
        <Card style={{ width: '25vw', height: '33vh', margin: '5px'}}>
            <CardHeader 
                avatar={<Avatar><SettingsIcon /></Avatar>}
                title={<Typography variant="body1" color='primary'>{obj.name ? obj.name : obj.id}</Typography>}
                subheader={obj.traits.join()}
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
                {getObjectThumbnail(obj)}
            </CardContent>
        </Card>
        <Dialog open={inDeleteDialog} onClose={onDeleteDialogCancel} aria-labelledby="alert-dialog-title" aria-describedby="alert-dialog-description">
            <DialogTitle id="alert-dialog-title">{"Are you sure you want to delete object?"}</DialogTitle>
            <DialogContent>
                <DialogContentText id="alert-dialog-description">
                    This will delete object.
                </DialogContentText>
            </DialogContent>
            <DialogActions>
                <Button variant="contained" onClick={onDeleteDialogCancel} color="primary">
                    No
                </Button>
                <Button variant="contained" onClick={onDeleteDialogOk} color="primary" autoFocus>
                    Yes
                </Button>
            </DialogActions>
        </Dialog>
        <Dialog open={inEditDialog} onClose={onEditDialogCancel} aria-labelledby="alert-dialog-title" aria-describedby="alert-dialog-description">
            <DialogContent>
                <ObjectProperties height = '70vh' width='50vh' object={object} onChange={onObjectChanged} traits={obj.traits}/>
            </DialogContent>
            <DialogActions>
                <Button variant="contained" onClick={onEditDialogCancel} color="primary">
                    Cancel
                </Button>
                <Button variant="contained" onClick={onEditDialogOk} color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>
        </Dialog>
        </div>
    );
}

export default ObjectView;