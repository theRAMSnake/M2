import MateriaRequest from '../modules/materia_request'
import React from 'react';
import '../../css/Calendar.css';
import { Calendar, utils } from "react-modern-calendar-datepicker";

import AssignmentTurnedInIcon from '@material-ui/icons/AssignmentTurnedIn';
import EventIcon from '@material-ui/icons/Event';

import {
    IconButton,
    Drawer,
    Typography,
    Grid
} from "@material-ui/core";

function extractItems(calendarItems, date)
{
    var start = new Date(date);
    start.setHours(0, 0, 0, 0);
    var startts = Math.floor(start / 1000);
    
    var end = new Date(date);
    end.setHours(23,59,59,999);
    var endts = Math.floor(end / 1000);
    
    var r = calendarItems.filter((x) => {
        return (Number(x.timestamp) >= startts) && (Number(x.timestamp) <= endts);
    });

    return r.sort((x, y) => { return Number(x.timestamp) - Number(y.timestamp); });
}

export default function CalendarCtrl(props)
{
    const items = props.items;

    const defaultItems = extractItems(items, new Date());

    const [selectedDate, setSelectedDate] = React.useState(utils().getToday());
    const [todayItems, setTodayItems] = React.useState(defaultItems);

    function onDateSelected(newDate)
    {
        setSelectedDate(newDate);
        var date = new Date();
        date.setFullYear(newDate.year, newDate.month - 1, newDate.day);
        var newItems = extractItems(items, date); 
        setTodayItems(newItems);
    }

    return <div>
            <Calendar value={selectedDate} onChange={onDateSelected}/>
            <div style={{ padding: 20 }}>
            <Grid alignItems='flex-start' direction='column' spacing={5}>
            {todayItems && todayItems.map((obj => {
                var dt = new Date(obj.timestamp * 1000);
                dt = new Date(dt.getTime() + dt.getTimezoneOffset() * 60000);

                return (<div>
                        {obj.entityType === "1" ? <AssignmentTurnedInIcon style={{paddingTop: '10px'}}/> : <EventIcon style={{paddingTop: '10px'}}/>} 
                        <Typography style={{display: "inline", paddingLeft: '10px'}} variant="h6">{dt.getHours() + ":" + dt.getMinutes().toString().padStart(2, "0") +" - " + obj.text}</Typography>
                    </div>);
                }))}
            </Grid>
            </div>
           </div>;
}