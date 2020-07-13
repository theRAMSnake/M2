import React, { useState } from 'react';

import {
    Dialog,
    DialogTitle,
    DialogContent,
    Button,
    DialogActions,
    FormControl,
    Select,
    RadioGroup,
    Radio,
    FormControlLabel,
    TextField
} from '@material-ui/core'

function AddJournalItemDialog(props)
{
    const [title, setTitle] = useState("");
    const [isPage, setIsPage] = useState("false");

    function onTitleChanged(e)
    {
        setTitle(e.target.value);
    }

    function selectedChange(e)
    {
        setIsPage(e.target.value);
    }

    return (
        <Dialog open={props.open} onClose={props.onCancel} aria-labelledby="dialog-title">
            <DialogTitle id="dialog-title">Create new journal item</DialogTitle>
            <DialogContent>
                <TextField id="id_title" label="Title" fullWidth  inputProps={{onChange: onTitleChanged}}/>
                <RadioGroup value={isPage} onChange={selectedChange}>
                    <FormControlLabel value="false" control={<Radio />} label="Folder" />
                    <FormControlLabel value="true" control={<Radio />} label="Page" />
                </RadioGroup>
            </DialogContent>
            <DialogActions>
                <Button onClick={props.onCancel} variant="contained" color="primary">
                    Cancel
                </Button>
                <Button onClick={() => props.onOk(title, isPage === "true")} variant="contained" color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>
        </Dialog>
    );
}

export default AddJournalItemDialog;