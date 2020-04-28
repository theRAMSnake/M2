import React, { useState } from 'react';

import {
    Card,
    CardContent,
    Typography,
    Divider
} from "@material-ui/core";

import SettingsIcon from '@material-ui/icons/Settings';

function ObjectView(props) 
{
    const obj = props.value;

    return (
        <Card>
            <CardContent>
                <div style={{display: 'flex', alignItems: 'center'}}>
                    <SettingsIcon fontSize="large" style={{margin:'5px'}}/>
                    <Typography variant="h6" color="primary">{obj.id}</Typography>
                </div>
                <Divider/>
                <Typography variant="body" color="textSecondary">{obj.traits.join()}</Typography>
                <Typography variant="body2" color="textSecondary"><pre id="json">{JSON.stringify(obj, null, ' ')}</pre></Typography>
            </CardContent>
        </Card>
    );
}

export default ObjectView;