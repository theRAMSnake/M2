import React from 'react';
import clsx from 'clsx';
import Auth from '../modules/auth'
import m3proxy from '../modules/m3proxy'
import materiaModel from '../modules/model'
import Materia from '../modules/materia_request'
import SearchBar from './SearchBar.jsx'
import ApiView from './ApiView.jsx'
import FinanceView from './FinanceView.jsx'
import JournalView from './JournalView.jsx'
import StrategyView from './StrategyView.jsx'
import IdeasView from './IdeasView.jsx'
import RewardView from './RewardView.jsx'
import QueryView from './QueryView.jsx'
import ProjectView from './ProjectView.jsx'
import CollectionView from './CollectionView.jsx'
import AddItemDialog from './AddItemDialog.jsx'
import CalendarCtrl from './CalendarCtrl.jsx'
import ContractsCtrl from './ContractsCtrl.jsx'
import VariableBurndown from './VariableBurndown.jsx'
import VariablePanel from './VariablePanel.jsx'
import ScriptHelper from '../modules/script_helper'

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
    CircularProgress,
    Backdrop
} from "@material-ui/core";

import MenuIcon from '@material-ui/icons/Menu';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
import ChevronRightIcon from '@material-ui/icons/ChevronRight';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import CalendarTodayIcon  from '@material-ui/icons/CalendarToday';
import MailIcon  from '@material-ui/icons/Mail';

import { createStyles, makeStyles, Theme } from '@material-ui/core/styles';
import MuiAlert from '@material-ui/lab/Alert';
import BadgetList from './BadgetList.jsx';

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
    if(!calendarItems)
    {
        return 0;
    }

    var curDate = new Date();
    curDate.setHours(23,59,59,999);
    var ts = Math.floor(curDate / 1000);

    var result = 0;
    var i = 0;
    for(; i < calendarItems.length; ++i)
    {
        var item = calendarItems[i];
        if(Number(item.timestamp) < ts)
        {
            result++;
        }
    }

    return result;
}

function MainPage(props) {

    const classes = useStyles();
    const [refreshCnt, setRefreshCnt] = React.useState(0);
    const [ldOpen, setldOpen] = React.useState(false);
    const [rdOpen, setrdOpen] = React.useState(false);
    const [contentType, setContentType] = React.useState("");
    const [projectName, setProjectName] = React.useState("");
    const [query, setQuery] = React.useState("");
    const [projects, setProjects] = React.useState([]);
    const [strategyPath, setStrategyPath] = React.useState("/");
    const [collectionName, setCollectionName] = React.useState("=");
    const [showAddDlg, setShowAddDlg] = React.useState(false);
    const [isInitialisation, setIsInitialisation] = React.useState(true);

    const [snackOpen, setSnackOpen] = React.useState(false);
    const [lastError, setLastError] = React.useState('');

    function refresh()
    {
        setRefreshCnt(refreshCnt+1);
    }

    m3proxy.initialize(() => {setIsInitialisation(false)});
    Materia.setGlobalErrorHandler((err) => {setLastError(err); setSnackOpen(true);});
    materiaModel.init();
    materiaModel.setOnUpdateCallback(refresh);

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

        else if(ct == "strategy")
            return (<StrategyView initPath={strategyPath}/>);

        else if(ct == "reward")
            return (<RewardView/>);

        else if(ct == "ideas")
            return (<IdeasView/>);

        else if(ct == "collection")
            return (<CollectionView colName={collectionName}/>);

        else if(ct == "project")
            return (<ProjectView projName={projectName}/>);
    }

    function logout_clicked(e) {
        e.preventDefault();
        Auth.deauthenticateUser();
    }

  React.useEffect(() => {
    ScriptHelper.loadCollection("projects", (data) => {
        setProjects(data);
    }, (error) => {
    });
  }, []);

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
        else if(index == 3)
        {
            setContentType("strategy");
        }
        else if(index == 4)
        {
            setContentType("reward");
        }
        else if(index == 5)
        {
            setContentType("ideas");
        }
    }

    function projectItemClicked(name)
    {
        setldOpen(false);
        setProjectName(name);
        setContentType("project");
    }

    function searchBarSubmit(text)
    {
        if(text.charAt(0) === "/")
        {
            setContentType("strategy");
            setStrategyPath(text);
        }
        else if(text.charAt(0) === "=")
        {
            setContentType("collection");
            setCollectionName(text);
        }
        else
        {
            setContentType("query");
            setQuery(text);
        }
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

    function getIncomeColor(value)
    {
        if(value < 0)
        {
            return "red";
        }
        else
        {
            return "primary";
        }
    }

    return (
        <div>
            <Backdrop open={isInitialisation}><CircularProgress color="inherit"/></Backdrop>
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
                        Materia v3.7.59
                    </Typography>
                    <ContractsCtrl/>
                    <SearchBar onSubmit={searchBarSubmit}/>
                    <div className={classes.grow} />
                    <VariablePanel value={materiaModel.getPrimaryFocus()} commit={materiaModel.setPrimaryFocus} length={250}/>
                    <VariablePanel value={materiaModel.getYearlyIncome().toString()} readonly length={60} color={getIncomeColor(materiaModel.getYearlyIncome())}/>
                    <VariableBurndown var={materiaModel.getWorkBurden()} commit={materiaModel.setWorkBurden}/>
                    <BadgetList id='inbox' icon={MailIcon}/>
                    <IconButton color="inherit" onClick={onAddClicked}>
                        <AddCircleOutlineIcon/>
                    </IconButton>
                    <IconButton color="inherit" onClick={handleRightDrawerOpen}>
                        <Badge badgeContent={calculateNumImportantCalendarItems(materiaModel.getCalendarItems())} color="secondary">
                            <CalendarTodayIcon/>
                        </Badge>
                    </IconButton>
                    <Button variant="contained" color="primary" size="small" onClick={logout_clicked}>Logout</Button>
                </Toolbar>
            </AppBar>
            <Snackbar open={snackOpen} autoHideDuration={6000} onClose={handleSnackClose}>
                <MuiAlert elevation={6} variant="filled" onClose={handleSnackClose} severity="error">
                    {lastError}
                </MuiAlert>
            </Snackbar>
            {showAddDlg && <AddItemDialog onClose={onAddDialogClosed}/>}
            <Divider/>
            <Grid  style={{paddingTop:'5px'}} container direction="column" justify="center" alignItems="center">
                <Typography variant="h4" className={classes.mar} color="primary">
                    {materiaModel.getTod()}
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
                    {['API', 'Finance', 'Journal', 'Strategy', 'Reward', 'Ideas'].map((text, index) => (
                    <ListItem button key={text} onClick={() => {menuItemClicked(index)}}>
                        <ListItemText primary={text} />
                    </ListItem>
                    ))}
                </List>
                <Divider />
                <List>
                    {projects.map((p, index) => (
                    <ListItem button key={p.name} onClick={() => {projectItemClicked(p.name)}}>
                        <ListItemText primary={p.name} />
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
                {materiaModel.getCalendarItems() && <CalendarCtrl items={materiaModel.getCalendarItems()} onChanged={materiaModel.invalidateCalendar}/>}
            </Drawer>
        </div>
    );
}

export default MainPage;
