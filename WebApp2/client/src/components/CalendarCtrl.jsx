import MateriaRequest from '../modules/materia_request'
import React from 'react';
import '../../css/Calendar.css';
import { Calendar, utils } from "react-modern-calendar-datepicker";

import AssignmentTurnedInIcon from '@material-ui/icons/AssignmentTurnedIn';
import EventIcon from '@material-ui/icons/Event';
import DeleteIcon from '@material-ui/icons/Delete';
import DoneIcon from '@material-ui/icons/Done';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';

import AddItemDialog from './AddItemDialog.jsx'
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'

import {
    IconButton,
    ListItemText,
    ListItemSecondaryAction,
    ListItemIcon,
    List,
    ListItem,
    Grid
} from "@material-ui/core";

function toUTC(date)
{
    return new Date(date.getTime() - date.getTimezoneOffset() * 60000);
}

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
    const [showAddDlg, setShowAddDlg] = React.useState(false);

    const [inDeleteDialog, setInDeleteDialog] = React.useState(false);
    const [inCompleteDialog, setInCompleteDialog] = React.useState(false);
    const [focusedItemIndex, setFocusedItemIndex] = React.useState(-1);

    function onDateSelected(newDate)
    {
        setSelectedDate(newDate);
        var date = new Date();
        date.setFullYear(newDate.year, newDate.month - 1, newDate.day);
        var newItems = extractItems(items, date); 
        setTodayItems(newItems);
    }

    function getInitTs()
    {
        var date = new Date();
        date.setFullYear(selectedDate.year, selectedDate.month - 1, selectedDate.day);
        date.setHours(9, 0);
        date = toUTC(date);

        return Math.floor(date / 1000);
    }

    function onAddDialogClosed()
    {
        setShowAddDlg(false);
        props.onChanged();
    }

    function onAddClicked()
    {
        setShowAddDlg(true);
    }

    function prepareDelete(index)
    {
        setInDeleteDialog(true);
        setFocusedItemIndex(index);
    }

    function prepareComplete(index)
    {
        setInCompleteDialog(true);
        setFocusedItemIndex(index);
    }

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
        setFocusedItemIndex(-1);
    }

    function onCompleteDialogCancel()
    {
        setInCompleteDialog(false);
        setFocusedItemIndex(-1);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        MateriaRequest.postDelete(todayItems[focusedItemIndex].id);
        setFocusedItemIndex(-1);
        props.onChanged();
    }

    function onCompleteDialogOk()
    {
        setInCompleteDialog(false);
        MateriaRequest.postComplete(todayItems[focusedItemIndex].id);
        setFocusedItemIndex(-1);
        props.onChanged();
    }

    return <div>
            {showAddDlg && <AddItemDialog onClose={onAddDialogClosed} selectedType="calendar_item" init={{timestamp: getInitTs()}}/>}
            <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
            <ConfirmationDialog open={inCompleteDialog} question="complete" caption="confirm completion" onNo={onCompleteDialogCancel} onYes={onCompleteDialogOk} />
            <Grid container direction="column" justify="space-around" alignItems="center">
                <Calendar value={selectedDate} onChange={onDateSelected}/>
                <IconButton edge="end" aria-label="complete" onClick={onAddClicked}>
                    <AddCircleOutlineIcon/>
                </IconButton>
            </Grid>
            <div style={{ padding: 20 }}>
            <List>
            {todayItems && todayItems.map(((obj, index) => {
                var dt = new Date(obj.timestamp * 1000);
                dt = new Date(dt.getTime() + dt.getTimezoneOffset() * 60000);

                return (<ListItem button>
                        <ListItemIcon>
                            {obj.entityType === "1" ? <AssignmentTurnedInIcon/> : <EventIcon/>} 
                        </ListItemIcon>
                        <ListItemText primary={dt.getHours() + ":" + dt.getMinutes().toString().padStart(2, "0") +" - " + obj.text}/>    
                        <ListItemSecondaryAction>
                            <IconButton edge="end" size='small' aria-label="complete" onClick={() => prepareComplete(index)}>
                                <DoneIcon fontSize='small'/>
                            </IconButton>
                            <IconButton edge="end" size='small' aria-label="delete" onClick={() => prepareDelete(index)}>
                                <DeleteIcon fontSize='small'/>
                            </IconButton>
                        </ListItemSecondaryAction>
                    </ListItem>);
                }))}
            </List>
            </div>
           </div>;
}