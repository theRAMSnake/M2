import React from 'react';
import Grid from '@material-ui/core/Grid';
import materiaModel from '../modules/model'
  
import {
    Input,
    FormControl,
    Select
} from '@material-ui/core'

export default function MoneyCtrl(props) 
{
    function getCurrency(s)
    {
        return s.substring(s.length - 3);
    }

    function getValue(s)
    {
        return s.substring(0, s.length - 3);
    }

    function handleCurrencyChange(e)
    {
        props.onChange(props.id, getValue(props.value) + e.target.value);
    }

    function handleMoneyChange(e)
    {
        var fl = parseFloat(e.target.value);
        if(fl)
        {
            props.onChange(props.id, fl.toFixed(2) + getCurrency(props.value));
        }
    }
    return (<Grid container justify="center" alignItems="center">
                <FormControl style={{width:"75px"}}>
                    <Input
                        inputProps={{style:{textAlign:"right"}}}
                        value={getValue(props.value)}
                        onChange={handleMoneyChange}
                    />
                </FormControl>
                <Select
                    native
                    value={getCurrency(props.value)}
                    onChange={handleCurrencyChange}
                    >
                    {materiaModel.getCurrencies().map((obj, index) => <option aria-label="None" value={obj.name} key={index} >{obj.name}</option>)}
                </Select>
            </Grid>);
}
