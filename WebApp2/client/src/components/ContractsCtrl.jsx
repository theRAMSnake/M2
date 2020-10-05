import Materia from '../modules/materia_request'
import React from 'react';
import { withStyles, makeStyles } from '@material-ui/core/styles';
import Tooltip from '@material-ui/core/Tooltip';
import Typography from '@material-ui/core/Typography';
import Grid from '@material-ui/core/Grid';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'

import {
    IconButton
} from "@material-ui/core";

import ClearIcon from '@material-ui/icons/Clear';
import ConfirmationNumberIcon from '@material-ui/icons/ConfirmationNumber';
import HourglassEmptyIcon from '@material-ui/icons/HourglassEmpty';
import AssistantPhotoIcon from '@material-ui/icons/AssistantPhoto';

const HtmlTooltip = withStyles((theme) => ({
    tooltip: {
      backgroundColor: '#424242',
      color: '#ffffff',
      minWidth: 250,
      fontSize: theme.typography.pxToRem(12)
    },
  }))(Tooltip);

export default function ContractsCtrl(props) 
{
    const [ctr, setCtr] = React.useState({object_list: []});
    const [init, setInit] = React.useState(false);
    const [inEditDialog, setInEditDialog] = React.useState(false);
    const [inDeleteDialog, setInDeleteDialog] = React.useState(false);
    const [activeC, setActiveC] = React.useState(null);

    if(!init)
    {
        const req = {
            operation: "query",
            filter: "IS(reward_contract)"
        };
        
        Materia.exec(req, (r) => {
            setCtr(r.object_list);
        });

        setInit(true);
    }

    function prepareEditDialog(contract)
    {
        setInEditDialog(true);
        setActiveC(contract);
    }

    function prepareDeleteDialog(contract)
    {
        setInDeleteDialog(true);
        setActiveC(contract);
    }

    function handleDeleteDialogFinished()
    {
        Materia.postDelete(activeC.id);
        setInDeleteDialog(false);
        var i = 0;
        for(; i < ctr.length; ++i)
        {
            if(ctr[i].id === activeC.id)
            {
                ctr.splice(i, 1);
                update();
                break;
            }
        }
    }

    function handleDialogCanceled()
    {
        setInEditDialog(false);
        setInDeleteDialog(false);
    }

    function update()
    {
        setCtr(JSON.parse(JSON.stringify(ctr)));
    }

    function handleEditDialogFinished(text)
    {
        var val = parseInt(text);
        if(!isNaN(val))
        {
            var total = parseInt(activeC.score) + val;
            setInEditDialog(false);
            activeC.score = total;
            Materia.postEdit(activeC.id, JSON.stringify(activeC));

            var i = 0;
            for(; i < ctr.length; ++i)
            {
                if(ctr[i].id === activeC.id)
                {
                    ctr[i].score = total;
                    update();
                }
            }
        }
    }

    function createContractItem(contract, margin)
    {
        if(contract)
        {
            return (
                <HtmlTooltip
                    title={
                    <React.Fragment>
                        <Typography color="inherit"><center>{contract.caption}</center></Typography>
                        <Grid container direction="row" justify="space-around" alignItems="center">
                            <div>
                                <HourglassEmptyIcon fontSize="small" style={{marginTop: 5}}/>
                                <Typography color="inherit" style={{display: 'inline', paddingLeft: 5}}>{contract.daysLeft}</Typography>
                            </div>
                            <div>
                                <AddCircleOutlineIcon fontSize="small" style={{marginTop: 5}}/>
                                <Typography color="inherit" style={{display: 'inline', paddingLeft: 5}}>{contract.reward}</Typography>
                            </div>
                            <div>
                                <AssistantPhotoIcon fontSize="small" style={{marginTop: 5}}/>
                                <Typography color="inherit" style={{display: 'inline', paddingLeft: 5}}>{contract.score}/{contract.goal}</Typography>
                            </div>
                        </Grid>
                    </React.Fragment>
                    }
                >
                    <IconButton 
                        style={{marginLeft: margin}} 
                        onContextMenu={(e) => {e.preventDefault(); prepareDeleteDialog(contract);}} 
                        onClick={() => prepareEditDialog(contract)}>
                            <ConfirmationNumberIcon/>
                    </IconButton>
                </HtmlTooltip>);
        }
        return <IconButton style={{marginLeft: margin}}><ClearIcon /></IconButton>;
    }

    return <div>
                {createContractItem(ctr[0], 10)}
                {createContractItem(ctr[1], -10)}
                {createContractItem(ctr[2], -10)}
                {inEditDialog && <TextQueryDialog text="" onFinished={handleEditDialogFinished} onCanceled={handleDialogCanceled}/>}
                <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={handleDialogCanceled} onYes={handleDeleteDialogFinished} />
            </div>;
}