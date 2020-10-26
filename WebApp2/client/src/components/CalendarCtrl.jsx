import Materia from '../modules/materia_request'
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
import GenericObjectDialog from './dialogs/GenericObjectDialog.jsx'

import {
    IconButton,
    ListItemText,
    ListItemSecondaryAction,
    ListItemIcon,
    List,
    ListItem,
    Grid,
    Typography
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
        return ( Number(x.timestamp) >= startts) && (Number(x.timestamp) <= endts);
    });

    console.log("extract");

    return r.sort((x, y) => { return x.urgency == y.urgency ? Number(x.timestamp) - Number(y.timestamp) : y.urgency - x.urgency });
}

export default function CalendarCtrl(props)
{
    const items = props.items;

    const [selectedDate, setSelectedDate] = React.useState(utils().getToday());
    const [todayItems, setTodayItems] = React.useState(null);
    const [src, setSrc] = React.useState(null);

    if(src != items)
    {
        setSrc(items);
        var date = new Date();
        date.setFullYear(selectedDate.year, selectedDate.month - 1, selectedDate.day);
        setTodayItems(extractItems(items, date));
    }

    const [showAddDlg, setShowAddDlg] = React.useState(false);

    const [inDeleteDialog, setInDeleteDialog] = React.useState(false);
    const [inEditDialog, setInEditDialog] = React.useState(false);
    const [inCompleteDialog, setInCompleteDialog] = React.useState(false);
    const [focusedItemIndex, setFocusedItemIndex] = React.useState(-1);
    const [objectInEdit, setObjectInEdit] = React.useState(null);

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

    function prepareEdit(index)
    {
        setInEditDialog(true);
        setObjectInEdit(todayItems[index]);
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

    function onEditDialogCancel()
    {
        setInDeleteDialog(false);
        setObjectInEdit(null);
    }

    function onObjectChanged(obj)
    {
        setObjectInEdit(obj);
    }

    function onEditDialogOk()
    {
        setInDeleteDialog(false);
        Materia.sendEdit(objectInEdit.id, JSON.stringify(objectInEdit), (resp) => {props.onChanged()});
        setObjectInEdit(null);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        Materia.sendDelete(todayItems[focusedItemIndex].id, (resp) => {props.onChanged()});
        setFocusedItemIndex(-1);
    }

    function onCompleteDialogOk()
    {
        setInCompleteDialog(false);
        Materia.sendComplete(todayItems[focusedItemIndex].id, (resp) => {props.onChanged()});
        setFocusedItemIndex(-1);
    }

    return <div>
            {showAddDlg && <AddItemDialog onClose={onAddDialogClosed} selectedType="calendar_item" init={{timestamp: getInitTs()}}/>}
            <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
            <ConfirmationDialog open={inCompleteDialog} question="complete" caption="confirm completion" onNo={onCompleteDialogCancel} onYes={onCompleteDialogOk} />
            {objectInEdit && <GenericObjectDialog open={inEditDialog} onCancel={onEditDialogCancel} onOk={onEditDialogOk} onChange={onObjectChanged} object={objectInEdit} />}
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

                return (<ListItem button key={obj.id} onClick={() => prepareEdit(index)}>
                        <ListItemIcon>
                            {obj.entityType === "0" ? <EventIcon/> : <AssignmentTurnedInIcon/>} 
                        </ListItemIcon>
                        <ListItemText disableTypography primary={
                            <Typography variant="body1" style={{ color: obj.urgency == 1 ? '#ff2929' : '#FFFFFF'}}>
                                {dt.getHours() + ":" + dt.getMinutes().toString().padStart(2, "0") +" - " + obj.text}
                            </Typography>}/>    
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