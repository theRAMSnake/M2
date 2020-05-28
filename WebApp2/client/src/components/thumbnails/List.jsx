import React, { useState } from 'react';
import ListIcon from '@material-ui/icons/List';

export default function ListThumbnail(props)
{
    return (
        <div style={{
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center'
        }}>
            <ListIcon style={{ fontSize: 64 }} />
        </div>
    );
}
