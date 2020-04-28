import React from 'react';
import clsx from 'clsx';
import Auth from '../modules/Auth'
import SearchBar from './SearchBar.jsx'
import ApiView from './ApiView.jsx'
import QueryView from './QueryView.jsx'

import {
    AppBar,
    Toolbar,
    Button,
    Typography,
    Drawer,
    IconButton,
    Divider,
    List,
    ListItem,
    ListItemText
} from "@material-ui/core";

import MenuIcon from '@material-ui/icons/Menu';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';

import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';

const drawerWidth = 240;

const useStyles = makeStyles((theme) =>
  createStyles({
    root: {
        flexGrow: 1,
    },
    grow: {
        flexGrow: 1,
    },
    hide: {
        display: 'none',
    },
    title: {
        display: 'none',
        color: 'LightGreen',
        [theme.breakpoints.up('sm')]: {
            display: 'block',
        },
    },
    drawer: {
        width: drawerWidth,
        flexShrink: 0,
    },
    drawerPaper: {
        width: drawerWidth,
    },
    drawerHeader: {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-start',
    },
    mar: {
        marginLeft: 10
    }
  }),
);

function MainPage(props) {
    
    const classes = useStyles();
    const [open, setOpen] = React.useState(false);
    const [contentType, setContentType] = React.useState("");
    const [query, setQuery] = React.useState("");

    function getContentView(ct) 
    {
        if(ct == "")
            return (<div/>);

        else if(ct == "api")
            return (<ApiView />);

        else if(ct == "query")
            return (<QueryView query={query}/>);
    }

    function logout_clicked(e) {
        e.preventDefault();
        Auth.deauthenticateUser();
    }

    function menuItemClicked(index)
    {
        if(index == 0)
        {
            setContentType("api");
            setOpen(false);
        }
    }

    function searchBarSubmit(text)
    {
        setQuery(text);
        setContentType("query");
    }

    const handleDrawerOpen = () => {
        setOpen(true);
    };

    const handleDrawerClose = () => {
        setOpen(false);
    };

    return (
        <div>
            <AppBar position="static" color="inherit">
                <Toolbar>
                    <IconButton
                        color="inherit"
                        aria-label="open drawer"
                        onClick={handleDrawerOpen}
                        edge="start"
                        className={clsx(open && classes.hide)}
                    >
                        <MenuIcon />
                    </IconButton>
                    <Typography variant="h6" className={classes.title}>
                        Materia
                    </Typography>
                    <SearchBar onSubmit={searchBarSubmit}/>
                    <div className={classes.grow} />
                    <Button variant="contained" color="primary" size="small" onClick={logout_clicked}>Logout</Button>
                </Toolbar>
            </AppBar>
            {getContentView(contentType)}
            <Drawer
                className={classes.drawer}
                variant="persistent"
                anchor="left"
                open={open}
                classes={{ paper: classes.drawerPaper }}
            >
                <div className={classes.drawerHeader}>
                    <MenuIcon className={classes.mar} />
                    <Typography variant="h6" className={classes.mar} color="primary">
                        Main menu
                    </Typography>
                    <div className={classes.grow} />
                    <IconButton onClick={handleDrawerClose}>
                        <ChevronLeftIcon />
                    </IconButton>
                </div>
                <Divider />
                <List>
                    {['API'].map((text, index) => (
                    <ListItem button key={text} onClick={() => {menuItemClicked(index)}}>
                        <ListItemText primary={text} />
                    </ListItem>
                    ))}
                </List>
            </Drawer>
        </div>
    );
}

export default MainPage;