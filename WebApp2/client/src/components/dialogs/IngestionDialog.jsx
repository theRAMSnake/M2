import React, { useState } from 'react';

import {
    Button,
    Dialog,
    DialogContent,
    DialogActions
} from "@material-ui/core";

import DateTimeCtrl from '../DateTimeCtrl.jsx'
import MoneyCtrl from '../MoneyCtrl.jsx'
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

function toUTC(date)
{
    return new Date(date.getTime() - date.getTimezoneOffset() * 60000);
}

function createDefault(f)
{
    var type = f.type;

    if(type === 'String') return "";
    if(type === 'Choice') return f.values[0].text;
    if(type === 'Bool') return false;
    if(type === 'Int') return 0;
    if(type === 'Double') return 0.0;
    if(type === 'Money') return "0.00EUR";
    if(type === 'Timestamp') return Math.floor(toUTC(new Date()) / 1000);
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
        result[ingestion[j].name] = createDefault(ingestion[j]);
    }

    return result;
}
export default function IngestionDialog(props)
{
    const [object, setObject] = useState(buildInitObject(props.ingestion, props.object));

    function handleFieldChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[e.target.id] = e.target.value;
        setObject(newObj);
    }

    function handleIntFieldChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[e.target.id] = parseInt(e.target.value);
        setObject(newObj);
    }

    function handleDoubleFieldChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[e.target.id] = parseFloat(e.target.value);
        setObject(newObj);
    }

    function handleDTChange(val, id)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[id] = val;
        setObject(newObj);
    }

    function handleMoney2Change(id, val)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[id] = val;
        setObject(newObj);
    }

    function handleChoiceChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[e.target.id] = e.target.value;
        setObject(newObj);
    }

    function handleCbChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(object));
        newObj[e.target.id] = e.target.checked ? 'true' : 'false';

        setObject(newObj);
    }


    function createPropCtrl(req)
    {
        if(req.type === 'String')
            return <TextField inputProps={{onChange: handleFieldChange}} value={object[req.name]} fullWidth id={req.name} label={req.name} />;
        if(req.type === 'Choice') {
            return <FormControl fullWidth style={{marginTop: '10px'}}>
                        <InputLabel htmlFor={req.name}>{req.name}</InputLabel>
                            <Select
                                native
                                value={object[req.name]}
                                onChange={handleChoiceChange}
                                inputProps={{
                                    name: req.name,
                                    id: req.name,
                                }}
                                >
                                {req.values.map((obj, index) => <option aria-label="None" value={obj.text} key={index} >{obj.text}</option>)}
                            </Select>
                    </FormControl>
        }
        if(req.type === 'Bool')
            return <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: handleCbChange}} id={req.name} checked={object[req.name].toString() === "true"} />} label={req.name} />
        if(req.type === 'Int')
            return <TextField inputProps={{onChange: handleIntFieldChange, type: 'number'}} value={object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'Double')
            return <TextField inputProps={{onChange: handleDoubleFieldChange, type: 'number', step:'any'}} value={object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'Timestamp')
        {
            return <DateTimeCtrl onChange={handleDTChange} value={object[req.name]} id={req.name}/>
        }
        if(req.type === 'Money')
        {
            return <MoneyCtrl onChange={handleMoney2Change} value={object[req.name]} id={req.name}/>
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
