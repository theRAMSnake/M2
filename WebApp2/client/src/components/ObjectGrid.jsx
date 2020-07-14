import React, { useState } from 'react';

import ObjectView from './ObjectView.jsx'

import {Grid} from '@material-ui/core'

function ObjectGrid(props)
{
    return (
        <Grid container alignItems='center' justify="center" direction='row'>
            {props.content.map((obj, index) => <ObjectView value={obj} key={obj.id}/>)}
        </Grid> 
    );
}

export default ObjectGrid;