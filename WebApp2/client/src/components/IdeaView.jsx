import React, { useState } from 'react';
import Materia from '../modules/materia_request'

import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import IdeaEditorDialog from './IdeaEditorDialog.jsx'

import {
    Card,
    Box,
    CardContent,
    CardHeader,
    Typography,
    Divider,
    IconButton,
    Avatar,
    Grid
} from "@material-ui/core";

import EmojiObjectsIcon from '@material-ui/icons/EmojiObjects';
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import EditIcon from '@material-ui/icons/Edit';

function IdeaView(props) 
{
    const [inDeleteDialog, setinDeleteDialog] = useState(false);
    const [inEditDialog, setinEditDialog] = useState(false);
    const [object, setObject] = useState(props.content);

    function deleteClicked(e)
    {
        setinDeleteDialog(true);
    }

    function editClicked(e)
    {
        setinEditDialog(true);
    }

    function onDeleteDialogCancel(e)
    {
        setinDeleteDialog(false);
    }

    function onEditDialogCancel(e)
    {
        setinEditDialog(false);
    }

    function onDeleteDialogOk(e)
    {
        setinDeleteDialog(false);
        Materia.postDelete(object.id);
        props.onDeleted();
    }
    
    function onEditDialogOk(title, htgs, content)
    {
        object.title = title;
        object.hashtags = htgs;
        object.content = content;

        setinEditDialog(false);

        setObject(JSON.parse(JSON.stringify(object)));
        Materia.postEdit(object.id, JSON.stringify(object));
    }

    return (
        <div>
        <Card style={{ minWidth: '25vw', minHeight: '24vh', margin: '5px'}}>
            <CardHeader 
                avatar={<Avatar><EmojiObjectsIcon /></Avatar>}
                title={<Typography variant="body1" color='secondary'>{object.title}</Typography>}
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
                <Grid
                    container
                    align="center"
                    justify="flex-start"
                    direction="column"
                >
                    <Box style={{ marginBottom: '50px', color: 'orange'}}>
                        <Typography variant="body2" color='inherit'>{object.hashtags && object.hashtags.join(" ")}</Typography>
                    </Box>
                    <Box>
                        <div dangerouslySetInnerHTML={{ __html: object.content }}></div>
                    </Box>
                    
                </Grid>
            </CardContent>
        </Card>
        <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
        {inEditDialog && <IdeaEditorDialog onOk={onEditDialogOk} onCancel={onEditDialogCancel} object={object}/>}
        </div>
    );
}

export default IdeaView;