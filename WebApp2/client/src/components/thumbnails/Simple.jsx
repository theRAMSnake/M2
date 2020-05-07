import React, { useState } from 'react';

import {
    Typography
} from "@material-ui/core";

export default function SimpleThumbnail(props)
{
    return (
        <div style={{
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center'
        }}>
            <Typography variant="h4" style={{color: 'lightgreen'}}>
                {props.value}
            </Typography>
        </div>
    );
}
