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
import DoneIcon from '@material-ui/icons/Done';
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
    const classes = useStyles();

    const [coins, setCoins] = useState(null);
    const [items, setItems] = useState(null);
    const [shopItems, setShopItems] = useState(null);
    const [snackOpen, setSnackOpen] = useState(false);
    const [snackText, setSnackText] = useState("");
    const [currentToken, setCurrentToken] = useState("");
    const [inDeleteDialog, setInDeleteDialog] = React.useState(false);
    const [inCompleteDialog, setInCompleteDialog] = React.useState(false);
    const [focusedItemId, setFocusedItemId] = React.useState(-1);

    if(coins == null)
    {
        materiaModel.getRewardCoins((coins) => { setCoins(coins); });
        materiaModel.getRewardItems((items) => { setItems(items); });
        materiaModel.getRewardShopItems((shopItems) => { setShopItems(shopItems); });
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

    function onCompleteDialogCancel()
    {
        setInCompleteDialog(false);
        setFocusedItemId(-1);
    }

    function prepareDelete(id)
    {
        setInDeleteDialog(true);
        setFocusedItemId(id);
    }

    function prepareComplete(id)
    {
        setInCompleteDialog(true);
        setFocusedItemId(id);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        Materia.sendDelete(focusedItemId, (x) => {
	    materiaModel.getRewardItems((items) => { setItems(items); });
	    setFocusedItemId(-1);
	});
    }

    function onCompleteDialogOk()
    {
        setInCompleteDialog(false);
        materiaModel.buyShopItem(focusedItemId, () => {
            materiaModel.getRewardCoins((coins) => { setCoins(coins); });
            materiaModel.getRewardItems((items) => { setItems(items); });
            materiaModel.getRewardShopItems((shopItems) => { setShopItems(shopItems); });
        });
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

    function CostView(props)
    {
        return <Box>
               {props.value.costRed != 0 && <Typography style={{color: "red", display: "inline"}}>{props.value.costRed + " "}</Typography>}
               {props.value.costBlue != 0 && <Typography style={{color: "blue", display: "inline"}}>{props.value.costBlue + " "}</Typography>}
               {props.value.costPurple != 0 && <Typography style={{color: "purple", display: "inline"}}>{props.value.costPurple + " "}</Typography>}
               {props.value.costYellow != 0 && <Typography style={{color: "yellow", display: "inline"}}>{props.value.costYellow + " "}</Typography>}
               {props.value.costGreen != 0 && <Typography style={{color: "green", display: "inline"}}>{props.value.costGreen + " "}</Typography>}
            </Box>;
    }

    function canAfford(item, coins)
    {
        return (parseInt(coins.Red) - parseInt(item.costRed) >= 0)
            && (parseInt(coins.Blue) - parseInt(item.costBlue) >= 0)
            && (parseInt(coins.Purple) - parseInt(item.costPurple) >= 0)
            && (parseInt(coins.Yellow) - parseInt(item.costYellow) >= 0)
            && (parseInt(coins.Green) - parseInt(item.costGreen) >= 0);
    }

    function getShopItemName(item)
    {
        if(item.amount == 1)
        {
            return item.name;
        }
        else
        {
            return item.name + " x" + item.amount;
        }
    }

    return (
        <div>
            <ConfirmationDialog open={inDeleteDialog} question="delete item" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
            <ConfirmationDialog open={inCompleteDialog} question="buy" caption="confirm buy" onNo={onCompleteDialogCancel} onYes={onCompleteDialogOk} />
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
                <Grid container direction="row" justify="space-around" alignItems="flex-start">
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
                    <List>
                    {shopItems && shopItems.map(x => {
                        return (<ListItem button>
                                  <ListItemText disableTypography primary={<Typography type="body1">{getShopItemName(x)}</Typography>}
                                                secondary={<CostView value={x}/>} />
                                  {canAfford(x, coins) && <ListItemSecondaryAction>
                                     <IconButton edge="end" size='small' aria-label="complete" onClick={() => prepareComplete(x.id)}>
                                        <DoneIcon fontSize='small'/>
                                     </IconButton>
                                  </ListItemSecondaryAction>}
                                </ListItem>);
                    })}
                    </List>
                </Grid>
            </Grid>
        </div>
    );
}

export default RewardView;
