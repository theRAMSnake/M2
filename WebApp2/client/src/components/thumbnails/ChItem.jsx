import React, { useState } from 'react';
import Bar from 'react-meter-bar';

import {
    Typography
} from "@material-ui/core";

export default function ChItemThumbnail(props)
{
    const pts = props.object.pointsNeeded;
    return (
        <div style={{width: '20vw'}}>
            <Typography variant="h4" style={{color: 'lightgreen'}}>
                {props.object.brief}
            </Typography>
            Level: {props.object.level}, {props.object.points}/{props.object.pointsNeeded}
            <div style={{paddingTop:'5px'}}>
                <Bar
                    labels={[0, Math.floor(pts * 0.25), Math.floor(pts * 0.5), Math.floor(pts * 0.75), pts]}
                    labelColor="primary"
                    progress={props.object.points / props.object.pointsNeeded * 100}
                    barColor="secondary"
                    seperatorColor="hotpink"
                    />
            </div>
        </div>
    );
}
