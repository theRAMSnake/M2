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

export default function VariablePanel(props) 
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
        setInEditDialog(false);
        props.commit(text);
    }

    return  <Paper elevation={3}>
                {inEditDialog && <TextQueryDialog text="" onFinished={handleEditDialogFinished} onCanceled={handleDialogCanceled}/>}
                <Box width={props.length} onClick={() => prepareEditDialog()}>
                    <Typography variant="h6" align="center" className={classes.unselectable}>
                        {props.value}
                    </Typography>
                </Box>
            </Paper>;
}