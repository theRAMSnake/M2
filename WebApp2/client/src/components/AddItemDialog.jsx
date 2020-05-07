import React, { useState } from 'react';
import m3proxy from '../modules/m3proxy'
import MateriaRequest from '../modules/materia_request'
import ObjectProperties from './ObjectProperties.jsx'

import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';

import {
    Dialog,
    DialogTitle,
    DialogContent,
    Button,
    DialogActions,
    FormControl,
    Select,
    InputLabel,
    TextField
} from '@material-ui/core'

const useStyles = makeStyles((theme) =>
  createStyles({
    formControl: {
      minWidth: 250,
    },
    selectEmpty: {
      marginTop: theme.spacing(2),
    },
  }),
);

function createDefault(type)
{
    if(type === 'string') return "";
    if(type === 'bool') return false;
    if(type === 'int') return 0;
    if(type === 'double') return 0.0;
}

function createObjectBody(traitName)
{
    var result = {};
    const trait = m3proxy.getTrait(traitName);
    if(trait.requires)
    {
        var j = 0;
        for (j = 0; j < trait.requires.length; j++)
        {
            result[trait.requires[j].field] = createDefault(trait.requires[j].type);
        }
    }

    console.log(result);
    return result;
}

function AddItemDialog(props)
{
    const [selectedTrait, setSelectedTrait] = useState('')
    const [selectedId, setSelectedId] = useState("")
    const [objectBody, setObjectBody] = useState()
    const [requesting, setRequesting] = useState(false)
    const classes = useStyles();

    function handleClose(e)
    {
        props.onClose(e);
    }

    function onOk(e)
    {
        var req = selectedId === "" ? {
            operation: "create",
            traits: [selectedTrait],
            params: objectBody
        } : {
            operation: "create",
            defined_id: selectedId,
            traits: [selectedTrait],
            params: objectBody
        };

        setRequesting(true);
        MateriaRequest.req(JSON.stringify(req), (rsp) => {

            var result = JSON.parse(rsp);
            if(result.id)
            {
                props.onClose(e);
            }
            else
            {
                setRequesting(false);
            }
        });
    }

    function selectedTraitChange(e)
    {
        setSelectedTrait(e.target.value);
        setObjectBody(createObjectBody(e.target.value));
    }

    function onIdChanged(e)
    {
        setSelectedId(e.target.value);
    }

    function onObjectChanged(e)
    {
        setObjectBody(e);
    }

    return (
        <Dialog open={true} onClose={handleClose} aria-labelledby="dialog-title">
            <DialogTitle id="dialog-title">Create new object</DialogTitle>
            <DialogContent>
                <FormControl className={classes.formControl} fullWidth>
                    <InputLabel htmlFor="trait">Trait</InputLabel>
                    <Select
                        native
                        value={selectedTrait}
                        onChange={selectedTraitChange}
                        inputProps={{
                            name: 'trait',
                            id: 'trait',
                        }}
                        >
                        <option aria-label="None" value="" />
                        {m3proxy.getTraits().map((obj) => <option aria-label="None" value={obj.name} >{obj.name}</option>)}
                    </Select>
                </FormControl>
                <TextField margin="dense" id="id_name" label="Id" fullWidth  inputProps={{onChange: onIdChanged}}/>
                <ObjectProperties height = '30vh' width='100%' onChange={onObjectChanged} traits={[selectedTrait]} object={objectBody}/>
            </DialogContent>
            {!requesting && <DialogActions>
                <Button onClick={handleClose} variant="contained" color="primary">
                    Cancel
                </Button>
                <Button onClick={onOk} variant="contained" color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>}
        </Dialog>
    );
}

export default AddItemDialog;