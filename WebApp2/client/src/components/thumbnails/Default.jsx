import React, { useState } from 'react';
import SettingsIcon from '@material-ui/icons/Settings';

import {
    Box
} from '@material-ui/core';

export default function DefaultThumbnail(props)
{
    return (
        <div style={{
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center'
        }}>
            <SettingsIcon style={{ fontSize: 64 }} />
        </div>
    );
}
