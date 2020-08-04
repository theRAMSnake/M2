import React from 'react';
import clsx from 'clsx';
import Auth from '../modules/auth'
import m3proxy from '../modules/m3proxy'
import Materia from '../modules/materia_request'
import SearchBar from './SearchBar.jsx'
import ApiView from './ApiView.jsx'
import FinanceView from './FinanceView.jsx'
import JournalView from './JournalView.jsx'
import QueryView from './QueryView.jsx'
import AddItemDialog from './AddItemDialog.jsx'
import InboxCtrl from './InboxCtrl.jsx'
import CalendarCtrl from './CalendarCtrl.jsx'

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
    Badge,
    ListItemText,
    Grid,
    Snackbar,
    Alert
} from "@material-ui/core";

import MenuIcon from '@material-ui/icons/Menu';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
import ChevronRightIcon from '@material-ui/icons/ChevronRight';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import CalendarTodayIcon  from '@material-ui/icons/CalendarToday';

import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';

const drawerWidth = 500;

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

function calculateNumImportantCalendarItems(calendarItems)
{
    var curDate = new Date();
    curDate.setHours(23,59,59,999);
    var ts = Math.floor(curDate / 1000);

    var result = 0;
    var i = 0;
    for(; i < calendarItems.object_list.length; ++i)
    {
        var item = calendarItems.object_list[i];
        if(Number(item.timestamp) < ts)
        {
            result++;
        }
    }

    return result;
}

function MainPage(props) {
    
    const classes = useStyles();
    const [ldOpen, setldOpen] = React.useState(false);
    const [rdOpen, setrdOpen] = React.useState(false);
    const [contentType, setContentType] = React.useState("");
    const [query, setQuery] = React.useState("");
    const [showAddDlg, setShowAddDlg] = React.useState(false);
    const [calendarItems, setCalendarItems] = React.useState({});
    const [numImportantCalendarItems, setNumImportantCalendarItems] = React.useState(0);
    const [tod, setTod] = React.useState("");

    const [snackOpen, setSnackOpen] = React.useState(0);
    const [lastError, setLastError] = React.useState('');

    function requestCalendarItems()
    {
        const req = {
            operation: "query",
            filter: "IS(calendar_item)"
        };

        Materia.exec(req, (r) => {
            var c = r;
            setCalendarItems(c);
            setNumImportantCalendarItems(calculateNumImportantCalendarItems(c));
        });

        {
            const req = {
                operation: "query",
                ids: ["tip_of_the_day"]
            };
    
            Materia.exec(req, (r) => {
                var c = r;
                setTod(c.object_list[0].value);
            });
        }
    }

    m3proxy.initialize();
    Materia.setGlobalErrorHandler((err) => {setLastError(err); setSnackOpen(true);});

    if(!calendarItems.object_list)
    {
        requestCalendarItems();
    }

    function onCalendarChanged()
    {
        requestCalendarItems();
    }

    function getContentView(ct) 
    {
        if(ct == "")
            return (<div/>);

        else if(ct == "api")
            return (<ApiView />);

        else if(ct == "query")
            return (<QueryView query={query}/>);

        else if(ct == "finance")
            return (<FinanceView/>);

        else if(ct == "journal")
            return (<JournalView/>);
    }

    function logout_clicked(e) {
        e.preventDefault();
        Auth.deauthenticateUser();
    }

    function menuItemClicked(index)
    {
        setldOpen(false);
        if(index == 0)
        {
            setContentType("api");
        }
        else if(index == 1)
        {
            setContentType("finance");
        }
        else if(index == 2)
        {
            setContentType("journal");
        }
    }

    function searchBarSubmit(text)
    {
        setContentType("query");
        setQuery(text);
    }

    const handleLeftDrawerOpen = () => {
        setldOpen(true);
    };

    const handleLeftDrawerClose = () => {
        setldOpen(false);
    };

    const handleRightDrawerOpen = () => {
        setrdOpen(true);
    };

    const handleRightDrawerClose = () => {
        setrdOpen(false);
    };

    function onAddClicked(e)
    {
        setShowAddDlg(true);
    } 

    function onAddDialogClosed(e)
    {
        setShowAddDlg(false);
    } 

    function handleSnackClose(e, r)
    {
        setSnackOpen(false);
    }

    return (
        <div>
            <AppBar position="static" color="inherit">
                <Toolbar>
                    <IconButton
                        color="inherit"
                        aria-label="open drawer"
                        onClick={handleLeftDrawerOpen}
                        edge="start"
                        className={clsx(ldOpen && classes.hide)}
                    >
                        <MenuIcon />
                    </IconButton>
                    <Typography variant="h6" className={classes.title}>
                        Materia
                    </Typography>
                    <SearchBar onSubmit={searchBarSubmit}/>
                    <div className={classes.grow} />
                    <InboxCtrl/>
                    <IconButton color="inherit" onClick={onAddClicked}>
                        <AddCircleOutlineIcon/>
                    </IconButton>
                    <IconButton color="inherit" onClick={handleRightDrawerOpen}>
                        <Badge badgeContent={numImportantCalendarItems} color="secondary">
                            <CalendarTodayIcon/>
                        </Badge>
                    </IconButton>
                    <Button variant="contained" color="primary" size="small" onClick={logout_clicked}>Logout</Button>
                </Toolbar>
            </AppBar>
            {/*<Snackbar open={snackOpen} autoHideDuration={6000} onClose={handleSnackClose}>
                <Alert onClose={handleSnackClose} severity="error">
                    {lastError}
                </Alert>
            </Snackbar>*/}
            {showAddDlg && <AddItemDialog onClose={onAddDialogClosed}/>}
            <Divider/>
            <Grid  style={{paddingTop:'5px'}} container direction="column" justify="center" alignItems="center">
                <Typography variant="h4" className={classes.mar} color="primary">
                    {tod}
                </Typography>
            </Grid>
            <Divider/>
            {getContentView(contentType)}
            <Drawer
                className={classes.drawer}
                variant="persistent"
                anchor="left"
                open={ldOpen}
                classes={{ paper: classes.drawerPaper }}
            >
                <div className={classes.drawerHeader}>
                    <MenuIcon className={classes.mar} />
                    <Typography variant="h6" className={classes.mar} color="secondary">
                        Main menu
                    </Typography>
                    <div className={classes.grow} />
                    <IconButton onClick={handleLeftDrawerClose}>
                        <ChevronLeftIcon />
                    </IconButton>
                </div>
                <Divider />
                <List>
                    {['API', 'Finance', 'Journal'].map((text, index) => (
                    <ListItem button key={text} onClick={() => {menuItemClicked(index)}}>
                        <ListItemText primary={text} />
                    </ListItem>
                    ))}
                </List>
            </Drawer>
            <Drawer
                className={classes.drawer}
                variant="persistent"
                anchor="right"
                open={rdOpen}
                classes={{ paper: classes.drawerPaper }}
            >
                <div className={classes.drawerHeader}>
                    <IconButton onClick={handleRightDrawerClose}>
                        <ChevronRightIcon />
                    </IconButton>
                    <Typography variant="h6" className={classes.mar} color="primary">
                        Calendar
                    </Typography>
                </div>
                <Divider />
                {calendarItems.object_list && <CalendarCtrl items={calendarItems.object_list} onChanged={onCalendarChanged}/>}
            </Drawer>
        </div>
    );
}

export default MainPage;