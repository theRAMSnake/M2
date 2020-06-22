import React, { useState } from 'react';

import {
    Typography
} from "@material-ui/core";

export default function SimpleThumbnail(props)
{
    return (
        <Typography variant="h4" style={{color: 'lightgreen'}}>
            {props.value}
        </Typography>
    );
}
