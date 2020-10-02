import React from 'react';
import Typography from '@material-ui/core/Typography';
import Breadcrumbs from '@material-ui/core/Breadcrumbs';
import Link from '@material-ui/core/Link';

export default function PathCtrl(props) 
{
    return (
        <div style={{marginTop: '5px', display:'inline'}}>
        <Breadcrumbs aria-label="breadcrumb" style={{display:'inline'}}>
            {props.pathList.map(x => {
                return <Link style={{display:'inline'}} color="inherit" href="/" onClick={(e) => {e.preventDefault(); props.onClick(x.id); }}>
                    {x.name}
                </Link>
            })}
            <Typography style={{display:'inline'}} color="textPrimary">{props.currentText}</Typography>
        </Breadcrumbs>
        </div>
        );
}