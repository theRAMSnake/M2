import React, { useState } from 'react';
import JSONInput from 'react-json-editor-ajrm';
import locale    from 'react-json-editor-ajrm/locale/en';
import m3proxy from '../modules/m3proxy'
import DateTimeCtrl from './DateTimeCtrl.jsx'
import MoneyCtrl from './MoneyCtrl.jsx'
import Switch from '@material-ui/core/Switch';
import {buildPinsTemplate, getPinOptions, applyPins, makePins} from '../modules/pins.js'

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

//DEPRECATED
function getRefOptions(refType)
{
    return m3proxy.getType(refType).objects;
}

function buildPropertiesTemplate(typename)
{
    var result = [];

    const type = m3proxy.getType(typename);
    if(type.fields)
    {
        var j = 0;
        for (j = 0; j < type.fields.length; j++)
        {
            result.push(type.fields[j]);
        }
    }

    return result;
}

export default function ObjectProperties(props)
{
    const propertiesTemplate = props.object ? buildPropertiesTemplate(props.object.typename) : [];
    const pins = props.pins ? props.pins : (props.object ? buildPinsTemplate(props.object.typename) : []);

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

    function handleChoiceChange(e)
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

    function handleMoney2Change(id, val)
    {
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[id] = val;

        props.onChange(newObj);
    }

    function handleReferenceChange(e)
    {
        //DEPRECATED
        let newObj = JSON.parse(JSON.stringify(props.object));
        newObj[e.target.id] = e.target.value;

        props.onChange(newObj);
    }

    function handlePinChange(e)
    {
        var newPins = JSON.parse(JSON.stringify(pins));

        for(var i = 0; i < newPins.length; ++i)
        {
            if(newPins[i].description === e.target.id)
            {
                newPins[i].value = e.target.value;
                break;
            }
        }

        props.onPinChanged(newPins);
    }

    function createPropCtrl(req)
    {
        if(props.blockedFields && props.blockedFields.includes(req.name))
        {
            return <div/>;
        }
        if(req.type === 'string' || req.type === 'period')
            return <TextField inputProps={{onChange: handleFieldChange}} value={props.object[req.name]} fullWidth id={req.name} label={req.name} />;
        if(req.type === 'bool')
            return <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: handleCbChange}} id={req.name} checked={props.object[req.name].toString() === "true"} />} label={req.name} />
        if(req.type === 'int')
            return <TextField inputProps={{onChange: handleIntFieldChange, type: 'number'}} value={props.object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'double')
            return <TextField inputProps={{onChange: handleDoubleFieldChange, type: 'number', step:'any'}} value={props.object[req.name]} id={req.name} fullWidth label={req.name} />;
        if(req.type === 'option')
            return <div/>;
        if(req.type === 'choice')
        {
            return <FormControl fullWidth style={{marginTop: '10px'}}>
                        <InputLabel htmlFor={req.name}>{req.name}</InputLabel>
                            <Select
                                native
                                value={props.object[req.name]}
                                onChange={handleChoiceChange}
                                inputProps={{
                                    name: req.name,
                                    id: req.name,
                                }}
                                >
                                {req.options.map((obj, index) => <option aria-label="None" value={obj} key={index} >{obj}</option>)}
                            </Select>
                    </FormControl>
        }
        if(req.type === 'timestamp')
        {
            return <DateTimeCtrl onChange={handleDTChange} value={props.object[req.name]} id={req.name}/>
        }
        if(req.type === 'money_v2')
        {
            return <MoneyCtrl onChange={handleMoney2Change} value={props.object[req.name]} id={req.name}/>
        }
        if(req.type === 'reference')
        {
            //Deprecated
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

    function createPinCtrl(pin)
    {
        if(props.blockedFields && props.blockedFields.includes(pin.description))
        {
            return <div/>;
        }
        return <FormControl fullWidth style={{marginTop: '10px'}}>
                    <InputLabel htmlFor={pin.description}>{pin.description}</InputLabel>
                        <Select
                            native
                            value={ pin.value }
                            onChange={handlePinChange}
                            inputProps={{
                                name: pin.description,
                                id: pin.description,
                            }}
                            >
                            {getPinOptions(pin.typeNameOther).map((obj, index) => <option aria-label="None" value={obj.id} key={index} >{obj.name || obj.id}</option>)}
                        </Select>
                </FormControl>
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
            {!jsonView && pins.length != 0 && pins.map((pin, index) => createPinCtrl(pin))}
        </div>
    );
}
