import React, { useState } from 'react';
import materiaModel from '../modules/model'
import Bar from 'react-meter-bar';

import {
    IconButton,
    Grid,
    ListItemText,
    ListItemIcon,
    List,
    ListItem,
    Typography
} from "@material-ui/core";

function RewardView(props) 
{
    const PTS_PER_CHEST = 2500;

    const [points, setPoints] = useState(null);
    const [items, setItems] = useState(null);
    
    if(points == null)
    {
        materiaModel.getRewardPoints((pts) => { setPoints(pts); });
        materiaModel.getRewardItems((items) => { setItems(items); });
    }
    
    function getNumChests()
    {
        return items.filter(x => x.name === "Chest").length;
    }

    function getOtherItems()
    {
        return items.filter(x => !(x.name === "Chest" || x.name.includes("Token")));
    }

    function getNumTokens(tok)
    {
        return items.find(x => x.name === tok + " Token").amount;
    }

    // Points, Tokens, Other items, Chests
    return (
        <div>
            <Grid container direction="column" justify="flex-start" alignItems="center">
                {points && <Bar
                    labels={[0, Math.floor(PTS_PER_CHEST * 0.25), Math.floor(PTS_PER_CHEST * 0.5), Math.floor(PTS_PER_CHEST * 0.75), PTS_PER_CHEST]}
                    labelColor="primary"
                    progress={points / PTS_PER_CHEST * 100}
                    barColor="secondary"
                    seperatorColor="hotpink"
                />}
                {items && 
                    <Grid container direction="row" justify="center" alignItems="flex-start">
                        <Typography>Chests: {getNumChests()} </Typography>
                        <Typography>Quest Tokens: {getNumTokens("Quest")} </Typography>
                        <Typography>PC Tokens: {getNumTokens("PC")} </Typography>
                        <Typography>VR Tokens: {getNumTokens("VR")} </Typography>
                    </Grid>
                }
                <List>
                {items && getOtherItems().map(x => { 
                    <ListItem button key={x.id}>
                        <ListItemText disableTypography primary={
                            <Typography variant="body1">
                                {x.name}
                            </Typography>}/>    
                    </ListItem>
                })}
                </List>
            </Grid>
        </div>
    );
}

export default RewardView;
