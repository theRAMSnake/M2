import React from 'react';
import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';

import {
    Typography,
    Paper,
    Box
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

export default function ConstantPanel(props) 
{
    const classes = useStyles();

    return  <Paper elevation={3}>
                <Box width={60} onClick={() => prepareEditDialog()}>
                    <Typography variant="h6" align="center" className={classes.unselectable}>
                        {props.value}
                    </Typography>
                </Box>
            </Paper>;
}