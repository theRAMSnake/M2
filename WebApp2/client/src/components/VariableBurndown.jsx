import React from 'react';
import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'

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

export default function VariableBurndown(props) 
{
    const classes = useStyles();

    const [inEditDialog, setInEditDialog] = React.useState(false);

    function prepareEditDialog()
    {
        setInEditDialog(true);
    }

    function handleDialogCanceled()
    {
        setInEditDialog(false);
    }

    function handleEditDialogFinished(text)
    {
        var val = parseInt(text);
        if(!isNaN(val))
        {
            var total = props.var - val;
            setInEditDialog(false);

            props.commit(total);
        }
    }

    return  <Paper elevation={3}>
                {inEditDialog && <TextQueryDialog text="" onFinished={handleEditDialogFinished} onCanceled={handleDialogCanceled}/>}
                <Box width={60} onClick={() => prepareEditDialog()}>
                    <Typography variant="h6" align="center" className={classes.unselectable}>
                        {props.var}
                    </Typography>
                </Box>
            </Paper>;
}