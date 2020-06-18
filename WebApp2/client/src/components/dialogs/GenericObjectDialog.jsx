import ObjectProperties from '../ObjectProperties.jsx'
import React, { useState } from 'react';

import {
    Button,
    Dialog,
    DialogContent,
    DialogActions
} from "@material-ui/core";

export default function GenericObjectDialog(props)
{
    return (<Dialog open={props.open} onClose={props.onCancel} aria-labelledby="alert-dialog-title" aria-describedby="alert-dialog-description">
            <DialogContent>
                <ObjectProperties height = '70vh' width='50vh' object={props.object} onChange={props.onChange} type={props.object.type}/>
            </DialogContent>
            <DialogActions>
                <Button variant="contained" onClick={props.onCancel} color="primary">
                    Cancel
                </Button>
                <Button variant="contained" onClick={props.onOk} color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>
        </Dialog>);
}