import MateriaRequest from '../modules/materia_request'
import React from 'react';
import '../../css/Calendar.css';
import { Calendar } from 'react-modern-calendar-datepicker';

import {
    IconButton,
    Drawer
} from "@material-ui/core";

export default function CalendarCtrl(props)
{
    return <Calendar />;
}