import React, { useState } from 'react';
import {
    Dialog,
    DialogActions,
    DialogContent,
    DialogContentText,
    DialogTitle,
    Button
} from '@material-ui/core';

export default function ConfirmationDialog(props)
{
    return (
        <Dialog open={props.open} onClose={props.onNo} aria-labelledby="alert-dialog-title" aria-describedby="alert-dialog-description">
            <DialogTitle id="alert-dialog-title">{"Are you sure you want to " + props.question + "?"}</DialogTitle>
            <DialogContent>
                <DialogContentText id="alert-dialog-description">
                    {props.caption}
                </DialogContentText>
            </DialogContent>
            <DialogActions>
                <Button variant="contained" onClick={props.onNo} color="primary">
                    No
                </Button>
                <Button variant="contained" onClick={props.onYes} color="primary" autoFocus>
                    Yes
                </Button>
            </DialogActions>
        </Dialog>
    );
}