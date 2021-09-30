import React, { useState } from 'react';
import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';
import materiaModel from '../modules/model'
import Materia from '../modules/materia_request'
import Bar from 'react-meter-bar';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'

import {
    IconButton,
    Grid,
    ListItemText,
    ListItemIcon,
    List,
    ListItem,
    Typography,
    Box,
    Paper,
    Snackbar,
    MuiAlert
} from "@material-ui/core";
import WorkIcon from '@material-ui/icons/Work';
import BuildIcon from '@material-ui/icons/Build';
import AccessibilityIcon from '@material-ui/icons/Accessibility';
import ImportantDevicesIcon from '@material-ui/icons/ImportantDevices';

const Alert = React.forwardRef(function Alert(props, ref) {
  return <MuiAlert elevation={6} ref={ref} variant="filled" {...props} />;
});

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

    const [points, setPoints] = useState(null);
    const [items, setItems] = useState(null);
    const [snackOpen, setSnackOpen] = useState(false);
    const [snackText, setSnackText] = useState("");
    const [currentToken, setCurrentToken] = useState("");
    
    if(points == null)
    {
        materiaModel.getRewardPoints((pts) => { setPoints(pts); });
        materiaModel.getRewardItems((items) => { setItems(items); });
    }
    
    function getNumChests()
    {
        return items.filter(x => x.name === "chest").length;
    }

    function getOtherItems()
    {
        return items.filter(x => !(x.name === "chest" || x.name.includes("Token")));
    }

    function getNumTokens(tok)
    {
        return items.find(x => x.name === tok + " Token").amount;
    }
    
    function useChest()
    {
        if(getNumChests != 0)
        {
            materiaModel.useChest(result => {
                materiaModel.getRewardItems((items) => { setItems(items); });
                setSnackText(result.chestType);
                setSnakeOpen(true);
            });
        }
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

    return (
        <div>
            <Snackbar open={snackOpen} autoHideDuration={6000} onClose={handleSnackClose}>
                <Alert onClose={handleSnackClose} severity="success" sx={{ width: '100%' }}>
                    {snackText}
                </Alert>
            </Snackbar>
            {currentToken.length > 0 && <TextQueryDialog text={getNumTokens(currentToken)} onFinished={handleEditDialogFinished} onCanceled={handleDialogCanceled}/>}
            <Grid container direction="column" justify="flex-start" alignItems="center">
                <div style={{width: '80vw', paddingTop:'5px'}}>
                    {points && <Bar
                        labels={[0, Math.floor(PTS_PER_CHEST * 0.25), Math.floor(PTS_PER_CHEST * 0.5), Math.floor(PTS_PER_CHEST * 0.75), PTS_PER_CHEST]}
                        labelColor="primary"
                        progress={points / PTS_PER_CHEST * 100}
                        barColor="secondary"
                        seperatorColor="hotpink"
                    />}
                </div>
                {items && 
                    <Grid container direction="row" justify="space-around" alignItems="flex-start">
                        <Box>
                            <WorkIcon style={{marginLeft:"18px"}}/>
                            <Paper elevation={3}>
                                <Box width={60} onClick={useChest}>
                                    <Typography variant="h6" align="center" className={classes.unselectable}>{getNumChests()}</Typography>
                                </Box>
                            </Paper>
                        </Box>
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
