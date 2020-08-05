import React, { useState } from 'react';

import ObjectView from './ObjectView.jsx'

import {Grid} from '@material-ui/core'

function ObjectGrid(props)
{
    return (
        <Grid container alignItems='center' justify="center" direction='row'>
            {/*props.content.length == 1 && props.content[0].typename === 'simple_list' ? <ObjectView value={props.content[0]} key={props.content[0].id} expanded/> : props.content.map((obj, index) => <ObjectView value={obj} key={obj.id}/>)*/}
            {props.content.map((obj, index) => <ObjectView value={obj} key={obj.id}/>)}
        </Grid> 
    );
}

export default ObjectGrid;