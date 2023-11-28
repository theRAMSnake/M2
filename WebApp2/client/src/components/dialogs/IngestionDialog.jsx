import React, { useState } from 'react';

import {
    Button,
    Dialog,
    DialogContent,
    DialogActions
} from "@material-ui/core";

import DateTimeCtrl from './DateTimeCtrl.jsx'
import MoneyCtrl from './MoneyCtrl.jsx'
import {
    FormControlLabel,
    TextField,
    Checkbox,
    FormControl,
    Select,
    InputLabel,
    Input,
    InputAdornment
} from '@material-ui/core'

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

function buildInitObject(ingestion, obj)
{
    if(obj) {
        return obj;
    }

    var result = {};

    var j = 0;
    for (j = 0; j < ingestion.length; j++)
    {
        result[ingestion[j].name] = createDefault(ingestion[j].type);
    }

    return result;
}
export default function IngestionDialog(props)
{
    const [object, setObject] = useState(buildInitObject(props.ingestion, props.object));

    function handleFieldChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = e.target.value;

        props.onChange(newObj);
    }

    function handleIntFieldChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = parseInt(e.target.value);

        props.onChange(newObj);
    }

    function handleDoubleFieldChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = parseFloat(e.target.value);

        props.onChange(newObj);
    }

    function handleDTChange(val, id)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[id] = val;

        props.onChange(newObj);
    }

    function handleMoney2Change(id, val)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[id] = val;

        props.onChange(newObj);
    }

    function createPropCtrl(req)
    {
        if(req.type === 'string' || req.type === 'period')
            return <TextField inputProps={{onChange: handleFieldChange}} value={props.object[req.name]} fullWidth id={req.name} label={req.name} />;
        if(req.type === 'bool')
            return <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: handleCbChange}} id={req.name} checked={props.object[req.name].toString() === "true"} />} label={req.name} />
        if(req.type === 'int')
            return <TextField inputProps={{onChange: handleIntFieldChange, type: 'number'}} value={props.object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'double')
            return <TextField inputProps={{onChange: handleDoubleFieldChange, type: 'number', step:'any'}} value={props.object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'timestamp')
        {
            return <DateTimeCtrl onChange={handleDTChange} value={props.object[req.name]} id={req.name}/>
        }
        if(req.type === 'money_v2')
        {
            return <MoneyCtrl onChange={handleMoney2Change} value={props.object[req.name]} id={req.name}/>
        }
    }

    return (<Dialog open={props.open} onClose={props.onCancel}>
            <DialogContent>
                <div height = '70vh' width='50vh'>
                    {props.ingestion.map((obj, index) => createPropCtrl(obj))}
                </div>
            </DialogContent>
            <DialogActions>
                <Button variant="contained" onClick={props.onCancel} color="primary">
                    Cancel
                </Button>
                <Button variant="contained" onClick={() => props.onOk(JSON.stringify(object))} color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>
        </Dialog>);
}
