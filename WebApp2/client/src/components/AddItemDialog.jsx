import React, { useState } from 'react';
import m3proxy from '../modules/m3proxy'
import MateriaRequest from '../modules/materia_request'
import JSONInput from 'react-json-editor-ajrm';
import locale    from 'react-json-editor-ajrm/locale/en';

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
    TextField,
    CircularProgress
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

function AddItemDialog(props)
{
    const [selectedTrait, setSelectedTrait] = useState("")
    const [selectedId, setSelectedId] = useState("")
    const [params, setParams] = useState("{\n}")
    const [error, setError] = useState("")
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
            params: JSON.parse(params)
        } : {
            operation: "create",
            defined_id: selectedId,
            traits: [selectedTrait],
            params: JSON.parse(params)
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
    }

    function onIdChanged(e)
    {
        setSelectedId(e.target.value);
    }

    function onParamsChanged(e)
    {
        setParams(e.json);
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
                        {m3proxy.getTraits().map((obj) => <option aria-label="None" value={obj.name} >{obj.name}</option>)}
                    </Select>
                </FormControl>
                <TextField margin="dense" id="id_name" label="Id" fullWidth  inputProps={{onChange: onIdChanged}}/>
                <JSONInput locale = { locale } height = '30vh' width='100%' onChange={onParamsChanged}/>
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