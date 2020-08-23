import Materia from '../modules/materia_request'
import React from 'react';
import { withStyles, makeStyles } from '@material-ui/core/styles';
import Tooltip from '@material-ui/core/Tooltip';
import Typography from '@material-ui/core/Typography';
import Grid from '@material-ui/core/Grid';
import Box from '@material-ui/core/Box';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'

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
    const [inDialog, setInDialog] = React.useState(false);
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

    function prepareDialog(contract)
    {
        setInDialog(true);
        setActiveC(contract);
    }

    function handleDialogCanceled()
    {
        setInDialog(false);
    }

    function update()
    {
        setCtr(JSON.parse(JSON.stringify(ctr)));
    }

    function handleDialogFinished(text)
    {
        var val = parseInt(text);
        if(!isNaN(val))
        {
            var total = parseInt(activeC.score) + val;
            setInDialog(false);
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
                    <IconButton style={{marginLeft: margin}} onClick={() => prepareDialog(contract)}><ConfirmationNumberIcon/></IconButton>
                </HtmlTooltip>);
        }
        return <IconButton style={{marginLeft: margin}}><ClearIcon /></IconButton>;
    }

    return <div>
                {createContractItem(ctr[0], 10)}
                {createContractItem(ctr[1], -10)}
                {createContractItem(ctr[2], -10)}
                {inDialog && <TextQueryDialog text="" onFinished={handleDialogFinished} onCanceled={handleDialogCanceled}/>}
            </div>;
}