import React, { useState } from 'react';
import m3proxy from '../modules/m3proxy'
import Materia from '../modules/materia_request'
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

function toUTC(date)
{
    return new Date(date.getTime() - date.getTimezoneOffset() * 60000);
}

function fromUTC(date)
{

}

function createDefault(f)
{
    var type = f.type;

    if(type === 'string') return "";
    if(type === 'array') return [];
    if(type === 'bool') return false;
    if(type === 'int') return 0;
    if(type === 'double') return 0.0;
    if(type === 'choice') return f.options[0];
    if(type === 'money') return 0;
    if(type === 'money_v2') return "0.00EUR";
    if(type === 'reference') return "";
    if(type === 'timestamp') return Math.floor(toUTC(new Date()) / 1000);
}

function createObjectBody(typeName)
{
    var result = {};
    const type = m3proxy.getType(typeName);
    if(type.fields)
    {
        var j = 0;
        for (j = 0; j < type.fields.length; j++)
        {
            result[type.fields[j].name] = createDefault(type.fields[j]);
        }
    }

    result.typename = typeName;
    console.log(result);
    return result;
}

function initObjectBody(typeName, init)
{
    var result = {};
    const type = m3proxy.getType(typeName);
    if(type.fields)
    {
        var j = 0;
        for (j = 0; j < type.fields.length; j++)
        {
            result[type.fields[j].name] = createDefault(type.fields[j]);
        }
    }

    for(var f in init)
    {
        result[f] = init[f];
    }

    result.typename = typeName;

    console.log(result);
    return result;
}

function AddItemDialog(props)
{
    const [selectedType, setSelectedType] = useState(props.selectedType ? props.selectedType : '')
    const [selectedId, setSelectedId] = useState("")
    const [objectBody, setObjectBody] = useState(props.init ? initObjectBody(props.selectedType, props.init) : null)
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
            typename: selectedType,
            params: objectBody
        } : {
            operation: "create",
            defined_id: selectedId,
            typename: selectedType,
            params: objectBody
        };

        setRequesting(true);
        Materia.exec(req, (rsp) => {

            var result = rsp;
            if(result.result_id)
            {
                props.onClose(e);
            }
            else
            {
                setRequesting(false);
            }
        });
    }

    function selectedTypeChange(e)
    {
        setSelectedType(e.target.value);
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
                    <InputLabel htmlFor="type">Type</InputLabel>
                    <Select
                        native
                        value={selectedType}
                        onChange={selectedTypeChange}
                        inputProps={{
                            name: 'type',
                            id: 'type',
                        }}
                        >
                        <option aria-label="None" value="" />
                        {m3proxy.getTypes().map((obj) => <option aria-label="None" value={obj.name} >{obj.name}</option>)}
                    </Select>
                </FormControl>
                <TextField margin="dense" id="id_name" label="Id" fullWidth  inputProps={{onChange: onIdChanged}}/>
                <ObjectProperties height = '30vh' width='100%' onChange={onObjectChanged} type={selectedType} object={objectBody}/>
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
