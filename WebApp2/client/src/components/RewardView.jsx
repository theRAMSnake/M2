import React, { useState } from 'react';
import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';
import materiaModel from '../modules/model'
import Materia from '../modules/materia_request'
import Bar from 'react-meter-bar';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'

import {
    IconButton,
    Grid,
    ListItemText,
    ListItemIcon,
    List,
    ListItem,
    ListItemSecondaryAction,
    ListItemButton,
    Typography,
    Box,
    Paper,
    Snackbar
} from "@material-ui/core";
import WorkIcon from '@material-ui/icons/Work';
import DeleteIcon from '@material-ui/icons/Delete';
import BuildIcon from '@material-ui/icons/Build';
import AccessibilityIcon from '@material-ui/icons/Accessibility';
import ImportantDevicesIcon from '@material-ui/icons/ImportantDevices';

const useStyles = makeStyles((theme) =>
  createStyles({
    unselectable: {
        "-webkit-user-select": "none",
        "-webkit-touch-callout": "none",
        "-moz-user-select": "none",
        "-ms-user-select": "none",
        "user-select": "none"
      }
  }),
);

function RewardView(props)
{
    const PTS_PER_CHEST = 2500;
    const classes = useStyles();

    const [coins, setCoins] = useState(null);
    const [items, setItems] = useState(null);
    const [snackOpen, setSnackOpen] = useState(false);
    const [snackText, setSnackText] = useState("");
    const [currentToken, setCurrentToken] = useState("");
    const [inDeleteDialog, setInDeleteDialog] = React.useState(false);
    const [focusedItemId, setFocusedItemId] = React.useState(-1);

    if(coins == null)
    {
        materiaModel.getRewardCoins((coins) => { setCoins(coins); });
        materiaModel.getRewardItems((items) => { setItems(items); });
    }

    function getOtherItems()
    {
        return items.filter(x => !(x.name.includes("Token")));
    }

    function getNumTokens(tok)
    {
        return items.find(x => x.name === tok + " Token").amount;
    }

    function handleSnackClose()
    {
        setSnackOpen(false);
    }

    function prepareEditDialog(token)
    {
        setCurrentToken(token);
    }

    function handleDialogCanceled()
    {
        setCurrentToken("");
    }

    function handleEditDialogFinished(text)
    {
        var tk = items.find(x => x.name === currentToken + " Token");
        tk.amount = Number(text);

        Materia.postEdit(tk.id, JSON.stringify(tk));

        setCurrentToken("");
    }

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
        setFocusedItemId(-1);
    }

    function prepareDelete(id)
    {
        setInDeleteDialog(true);
        setFocusedItemId(id);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        Materia.sendDelete(focusedItemId);
        materiaModel.getRewardItems((items) => { setItems(items); });
        setFocusedItemId(-1);
    }

    function CoinsView(props)
    {
        return <Paper elevation={3} style={{marginTop: '10px', marginBottom: '10px'}}>
                    <Box width={60}>
                        <Typography variant="h6" align="center" style={{color: props.color}} className={classes.unselectable}>{props.value}</Typography>
                    </Box>
               </Paper>;
    }

    return (
        <div>
            <ConfirmationDialog open={inDeleteDialog} question="delete item" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
            {currentToken.length > 0 && <TextQueryDialog text={getNumTokens(currentToken)} onFinished={handleEditDialogFinished} onCanceled={handleDialogCanceled}/>}
            <Grid container direction="column" justify="flex-start" alignItems="center">
                {coins &&
                    <Grid container direction="row" justify="space-around" alignItems="flex-start">
                        <CoinsView value={coins.Red} color="red"/>
                        <CoinsView value={coins.Blue} color="blue"/>
                        <CoinsView value={coins.Purple} color="purple"/>
                        <CoinsView value={coins.Yellow} color="yellow"/>
                        <CoinsView value={coins.Green} color="green"/>
                    </Grid>
                }
            </Grid>
            <Grid container direction="column" justify="flex-start" alignItems="center">
                {items &&
                    <Grid container direction="row" justify="space-around" alignItems="flex-start">
                        <Box>
                            <BuildIcon style={{marginLeft:"18px"}}/>
                            <Paper elevation={3}>
                                <Box width={60} onClick={() => setCurrentToken("Quest")}>
                                    <Typography variant="h6" align="center" className={classes.unselectable}>{getNumTokens("Quest")}</Typography>
                                </Box>
                            </Paper>
                        </Box>
                        <Box>
                            <ImportantDevicesIcon style={{marginLeft:"18px"}}/>
                            <Paper elevation={3}>
                                <Box width={60} onClick={() => setCurrentToken("PC")}>
                                    <Typography variant="h6" align="center" className={classes.unselectable}>{getNumTokens("PC")}</Typography>
                                </Box>
                            </Paper>
                        </Box>
                        <Box>
                            <AccessibilityIcon style={{marginLeft:"18px"}}/>
                            <Paper elevation={3}>
                                <Box width={60} onClick={() => setCurrentToken("VR")}>
                                    <Typography variant="h6" align="center" className={classes.unselectable}>{getNumTokens("VR")}</Typography>
                                </Box>
                            </Paper>
                        </Box>
                    </Grid>
                }
                <List>
                {items && getOtherItems().map(x => {
                    return (<ListItem button>
                              <ListItemText primary={x.name} />
                                <ListItemSecondaryAction>
                                    <IconButton edge="end" size='small' aria-label="delete" onClick={() => prepareDelete(x.id)}>
                                        <DeleteIcon fontSize='small'/>
                                    </IconButton>
                                </ListItemSecondaryAction>
                            </ListItem>);
                })}
                </List>
            </Grid>
        </div>
    );
}

export default RewardView;
