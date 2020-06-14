import 'date-fns';
import DateFnsUtils from '@date-io/date-fns';
import React from 'react';
import Grid from '@material-ui/core/Grid';
import {
    MuiPickersUtilsProvider,
    KeyboardTimePicker,
    KeyboardDatePicker,
  } from '@material-ui/pickers';
  
export default function DateTimeCtrl(props) 
{
    return (<Grid container>
        <MuiPickersUtilsProvider utils={DateFnsUtils}>
            <KeyboardDatePicker
                margin="normal"
                id="date-picker-dialog"
                label="Date"
                format="dd/MM/yyyy"
                value={props.value * 1000}
                onChange={(d) => props.onChange(d, props.id)}
                KeyboardButtonProps={{
                    'aria-label': 'change date',
                }}
            />
            {<KeyboardTimePicker
                margin="normal"
                id="time-picker"
                label="Time"
                value={props.value * 1000}
                format="HH:mm"
                onChange={(d) => props.onChange(d, props.id)}
                KeyboardButtonProps={{
                    'aria-label': 'change time',
                }}
            />}
        </MuiPickersUtilsProvider>
  </Grid>);
}