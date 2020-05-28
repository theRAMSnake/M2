import React, { useState } from 'react';
import {
    Dialog
} from '@material-ui/core';

export default function SimpleDialog(props)
{
    return (
        <Dialog open={props.open} onClose={props.onClose}>
            {props.text}
        </Dialog>
    );
}