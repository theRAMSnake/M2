import React, { useState } from 'react';
import JSONInput from 'react-json-editor-ajrm'; 
import locale    from 'react-json-editor-ajrm/locale/en';
import m3proxy from '../modules/m3proxy'

import Switch from '@material-ui/core/Switch';

import {
    FormControlLabel,
    TextField,
    Checkbox
} from '@material-ui/core'

function buildPropertiesTemplate(traits)
{
    var result = [];

    var i = 0;
    for (i = 0; i < traits.length; i++)
    {
        const trait = m3proxy.getTrait(traits[i]);
        if(trait.requires)
        {
            var j = 0;
            for (j = 0; j < trait.requires.length; j++)
            {
                result.push(trait.requires[j]);
            }
        }
    }

    return result;
}

export default function ObjectProperties(props)
{
    const propertiesTemplate = buildPropertiesTemplate(props.traits);

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

    function createPropCtrl(req)
    {
        if(req.type === 'string') 
            return <TextField inputProps={{onChange: handleFieldChange}} value={props.object[req.field]} fullWidth id={req.field} label={req.field} />;
        if(req.type === 'bool') 
            return <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: handleCbChange}} id={req.field} checked={props.object[req.field]} />} label={req.field} />
        if(req.type === 'int') 
            return <TextField inputProps={{onChange: handleIntFieldChange, type: 'number'}} value={props.object[req.field]} id={req.field} fullWidth label={req.field} />;
        if(req.type === 'double') 
            return <TextField inputProps={{onChange: handleDoubleFieldChange, type: 'number', step:'any'}} value={props.object[req.field]} id={req.field} fullWidth label={req.field} />;
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
            {(jsonView || propertiesTemplate == 0) && <JSONInput locale = { locale } 
                height = {props.height} 
                width = {props.width} 
                onChange = {handleJsonChange}
                placeholder = {props.object}/>}
            {!jsonView && propertiesTemplate != 0 && propertiesTemplate.map((obj, index) => createPropCtrl(obj))}
        </div>
    );
}