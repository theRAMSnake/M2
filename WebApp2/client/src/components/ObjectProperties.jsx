import React, { useState } from 'react';
import JSONInput from 'react-json-editor-ajrm'; 
import locale    from 'react-json-editor-ajrm/locale/en';
import m3proxy from '../modules/m3proxy'
import DateTimeCtrl from './DateTimeCtrl.jsx'
import Switch from '@material-ui/core/Switch';

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

function getRefOptions(refType)
{
    return m3proxy.getType(refType).objects;
}

function buildPropertiesTemplate(typename)
{
    var result = [];

    console.log(typename);

    const type = m3proxy.getType(typename);
    if(type.fields)
    {
        var j = 0;
        for (j = 0; j < type.fields.length; j++)
        {
            result.push(type.fields[j]);
        }
    }

    console.log(result);

    return result;
}

export default function ObjectProperties(props)
{
    const propertiesTemplate = props.object ? buildPropertiesTemplate(props.object.typename) : [];

    const [jsonView, setJsonView] = useState(false);

    function handleSwitchChange(event)
    {
        setJsonView(event.target.checked);
    }

    function handleJsonChange(e)
    {
        props.onChange(JSON.parse(e.json));
    }

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

    function handleCbChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = e.target.checked;

        props.onChange(newObj);
    }

    function handleOptionChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = e.target.value;

        props.onChange(newObj);
    }

    function handleDTChange(val, id)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[id] = val;

        props.onChange(newObj);
    }

    function handleMoneyChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = Math.floor(parseFloat(e.target.value) * 100);

        props.onChange(newObj);
    }

    function handleReferenceChange(e)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = e.target.value;

        props.onChange(newObj);
    }

    function createPropCtrl(req)
    {
        console.log(req);
        if(req.type === 'string') 
            return <TextField inputProps={{onChange: handleFieldChange}} value={props.object[req.name]} fullWidth id={req.name} label={req.name} />;
        if(req.type === 'bool') 
            return <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: handleCbChange}} id={req.name} checked={props.object[req.name]} />} label={req.name} />
        if(req.type === 'int') 
            return <TextField inputProps={{onChange: handleIntFieldChange, type: 'number'}} value={props.object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'double') 
            return <TextField inputProps={{onChange: handleDoubleFieldChange, type: 'number', step:'any'}} value={props.object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'option')
        {
            return <FormControl fullWidth style={{marginTop: '10px'}}>
                        <InputLabel htmlFor={req.name}>{req.name}</InputLabel>
                            <Select
                                native
                                value={props.object[req.name]}
                                onChange={handleOptionChange}
                                inputProps={{
                                    name: req.name,
                                    id: req.name,
                                }}
                                >
                                {req.options.map((obj, index) => <option aria-label="None" value={index} key={index} >{obj}</option>)}
                            </Select>
                    </FormControl>
        }
        if(req.type === 'timestamp')
        {
            return <DateTimeCtrl onChange={handleDTChange} value={props.object[req.name]} id={req.name}/>
        }
        if(req.type === 'money')
        {
            return <FormControl fullWidth>
                <InputLabel htmlFor="amount">Amount</InputLabel>
                <Input
                    id={req.name}
                    value={props.object[req.name] / 100.0}
                    onChange={handleMoneyChange}
                    type='number'
                    step='any'
                    startAdornment={<InputAdornment position="start">€</InputAdornment>}
                />
            </FormControl>
        }
        if(req.type === 'reference')
        {
            const val = props.object[req.name];
            if(val === "")
            {
                let newObj = JSON.parse(JSON.stringify(props.object));
                newObj[req.name] = getRefOptions(req.refType)[0].id;

                props.onChange(newObj);
            }
            return <FormControl fullWidth style={{marginTop: '10px'}}>
                        <InputLabel htmlFor={req.name}>{req.name}</InputLabel>
                            <Select
                                native
                                value={ props.object[req.name]}
                                onChange={handleReferenceChange}
                                inputProps={{
                                    name: req.name,
                                    id: req.name,
                                }}
                                >
                                {getRefOptions(req.refType).map((obj, index) => <option aria-label="None" value={obj.id} key={index} >{obj.name}</option>)}
                            </Select>
                    </FormControl>
        }
    }

    return (
        <div>
            <div style={{
                display: 'flex',
                alignItems: 'flex-end',
                justifyContent: 'flex-end'
            }}>
                <Switch
                    checked={jsonView}
                    onChange={handleSwitchChange}
                    disabled={propertiesTemplate.length == 0}
                    name="jsonView"
                    color='primary'
                    inputProps={{ 'aria-label': 'primary checkbox' }}
                />
            </div>
            {(jsonView || propertiesTemplate.length == 0) && <JSONInput locale = { locale } 
                height = {props.height} 
                width = {props.width} 
                onChange = {handleJsonChange}
                placeholder = {props.object}/>}
            {!jsonView && propertiesTemplate.length != 0 && propertiesTemplate.map((obj, index) => createPropCtrl(obj))}
        </div>
    );
}