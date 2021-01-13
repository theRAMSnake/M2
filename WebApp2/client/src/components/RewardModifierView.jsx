import React from 'react';
import { withStyles, createStyles, makeStyles, Theme } from '@material-ui/core/styles';
import Tooltip from '@material-ui/core/Tooltip';

import {
    Typography,
    Paper,
    Box,
    Grid
} from "@material-ui/core";

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

const HtmlTooltip = withStyles((theme) => ({
    tooltip: {
      backgroundColor: '#424242',
      color: '#ffffff',
      minWidth: 150,
      fontSize: theme.typography.pxToRem(12)
    },
  }))(Tooltip);

export default function RewardModifierView(props) 
{
    const classes = useStyles();

    var text = "";
    if(props.value)
    {
        var total = 0.0;
                
        props.value.forEach(x => {
            total += parseFloat(x.value)
        });

        text = parseInt((total * 100).toString()).toString() + "%";
    }

    //TODO:SORT
    return  <HtmlTooltip
                title={
                <React.Fragment>
                    <Grid container direction="column" justify="space-around" alignItems="left">
                        {props.value && props.value.map(x => {
                            return (<div>
                                <Typography style={{color: x.value > 0 ? "lightgreen" : "red", display: 'inline', paddingLeft: 5}}>{x.desc + 
                                    (x.value > 0 ? ": +" : ": ") + 
                                    parseInt((x.value * 100).toString()).toString() + "%"}</Typography>
                            </div>)
                        })}
                    </Grid>
                </React.Fragment>
                    }
                >
                <Paper elevation={3}>
                    <Box width={60} onClick={() => prepareEditDialog()}>
                        <Typography variant="h6" align="center" className={classes.unselectable}>
                            {text}
                        </Typography>
                    </Box>
                </Paper>
            </HtmlTooltip>;
}