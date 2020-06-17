import 'date-fns';
import DateFnsUtils from '@date-io/date-fns';
import React from 'react';
import Grid from '@material-ui/core/Grid';
import {
    MuiPickersUtilsProvider,
    KeyboardTimePicker,
    KeyboardDatePicker,
  } from '@material-ui/pickers';

  
function toUTC(date)
{
    //console.log("toUTC: " + date.getTime() + "->" + new Date(date.getTime() + new Date().getTimezoneOffset() * 60000).getTime());
    return new Date(date.getTime() - new Date().getTimezoneOffset() * 60000).getTime();
    //return date;
}

function fromUTC(ts)
{
    //console.log("fromUTC: " + ts + "->" + new Date(ts - new Date().getTimezoneOffset() * 60000).getTime());
    return new Date(ts + new Date().getTimezoneOffset() * 60000).getTime();
    //return ts;
}
  
export default function DateTimeCtrl(props) 
{
    return (<Grid container>
        <MuiPickersUtilsProvider utils={DateFnsUtils}>
            <KeyboardDatePicker
                margin="normal"
                id="date-picker-dialog"
                label="Date"
                format="dd/MM/yyyy"
                value={fromUTC(props.value * 1000)}
                onChange={(d) => props.onChange(toUTC(d) / 1000, props.id)}
                KeyboardButtonProps={{
                    'aria-label': 'change date',
                }}
            />
            {<KeyboardTimePicker
                margin="normal"
                id="time-picker"
                label="Time"
                value={fromUTC(props.value * 1000)}
                format="HH:mm"
                onChange={(d) => props.onChange(toUTC(d) / 1000, props.id)}
                KeyboardButtonProps={{
                    'aria-label': 'change time',
                }}
            />}
        </MuiPickersUtilsProvider>
  </Grid>);
}