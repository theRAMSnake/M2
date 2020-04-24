import React from 'react';
import Auth from '../modules/Auth'
import SearchBar from './SearchBar.jsx'

import {
    AppBar,
    Toolbar,
    Button,
    Typography
} from "@material-ui/core";

import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';

const useStyles = makeStyles((theme) =>
  createStyles({
    root: {
        flexGrow: 1,
    },
    grow: {
        flexGrow: 1,
    },
    title: {
        display: 'none',
        [theme.breakpoints.up('sm')]: {
            display: 'block',
        },
    },
  }),
);

function MainPage(props) {
    const classes = useStyles();

    function clicked(e) {
        e.preventDefault();
        Auth.deauthenticateUser();
    }

    return (
        <AppBar position="static" color="inherit">
            <Toolbar>
                <Typography variant="h6" className={classes.title}>
                    Materia
                </Typography>
                <SearchBar/>
                <div className={classes.grow} />
                <Button variant="contained" color="primary" size="small" onClick={clicked}>Logout</Button>
            </Toolbar>
        </AppBar>
    );
}

export default MainPage;