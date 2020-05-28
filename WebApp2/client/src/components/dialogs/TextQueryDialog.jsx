import React, { useState } from 'react';
import {
    Dialog,
    DialogActions,
    DialogContent,
    TextField,
    Button
} from '@material-ui/core';

export default function TextQueryDialog(props)
{
    const [text, setText] = useState(props.text);

    function onTextChanged(e)
    {
        setText(e.target.value);
    }

    return <Dialog fullWidth onClose={props.onCanceled} open={true}>
        <DialogContent>
            <TextField margin="dense" multiline id="id" label="text" value={text} fullWidth autoFocus inputProps={{onChange: onTextChanged}}/>
        </DialogContent>
        <DialogActions>
            <Button variant="contained" onClick={props.onCanceled} color="primary">
                Cancel
            </Button>
            <Button variant="contained" onClick={() => props.onFinished(text)} color="primary" >
                Ok
            </Button>
        </DialogActions>
    </Dialog>
}