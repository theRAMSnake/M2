import React, { useState } from 'react';
import materiaModel from '../modules/model'
import IdeaView from './IdeaView.jsx'
import IdeaEditorDialog from './IdeaEditorDialog.jsx'

import SearchIcon from '@material-ui/icons/Search';
import HelpOutlineIcon from '@material-ui/icons/HelpOutline';
import EmojiObjectsIcon from '@material-ui/icons/EmojiObjects';
import InputBase from '@material-ui/core/InputBase';
import { makeStyles, fade, Theme, createStyles } from '@material-ui/core/styles';

import {
    Box,
    IconButton,
    Grid,
    ListItemText,
    CircularProgress,
    Backdrop,
    ListItemIcon,
    List,
    ListItem,
    Typography
} from "@material-ui/core";

import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';

const useStyles = makeStyles((theme) =>
  createStyles({
    root: {
        flexGrow: 1,
    },
    search: {
        position: 'relative',
        borderRadius: theme.shape.borderRadius,
        backgroundColor: fade(theme.palette.common.white, 0.15),
        '&:hover': {
          backgroundColor: fade(theme.palette.common.white, 0.25),
        },
        marginRight: theme.spacing(1),
        marginLeft: 0,
        width: '100%',
        [theme.breakpoints.up('sm')]: {
          marginLeft: theme.spacing(1),
          width: 'auto',
        }
    },
    searchIcon: {
      padding: theme.spacing(0, 2),
      height: '100%',
      position: 'absolute',
      pointerEvents: 'none',
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'center',
    },
    inputRoot: {
      color: 'inherit',
    },
    inputInput: {
      padding: theme.spacing(1, 1, 1, 0),
      // vertical padding + font size from searchIcon
      paddingLeft: `calc(1em + ${theme.spacing(4)}px)`,
      transition: theme.transitions.create('width'),
      width: '100%'
    }
  }),
);

function IdeasView(props) 
{
    const classes = useStyles();

    const [updating, setUpdating] = useState(false);
    const [inAddDialog, setinAddDialog] = useState(false);
    const [curIdea, setCurIdea] = useState(null);
    const [searchText, setSearchText] = useState("");
    const [ideaList, setIdeaList] = useState(null);

    function updateSecondaryIdeaList(i, kw)
    {
        setUpdating(true);
        if(kw.length < 3)
        {
            materiaModel.getRelevantIdeas(i.id, (res)=>{
                setIdeaList(res);
                setUpdating(false);
            });
        }
        else
        {
            materiaModel.searchIdeas(kw, (res)=>{
                setIdeaList(res);
                setUpdating(false);
            });
        }
    }

    function selectRandomIdea()
    {
        materiaModel.getRandomIdea((i)=>{
            setCurIdea(i);
            updateSecondaryIdeaList(i, searchText);
        });
    }

    if(curIdea == null)
    {
        selectRandomIdea();
    }

    function onIdeaDeleted()
    {
        selectRandomIdea();
    }

    function onAddClicked()
    {
        setinAddDialog(true);
    }

    function onAddDialogCancel(e)
    {
        setinAddDialog(false);
    }
    
    function onAddDialogOk(title, htgs, content)
    {
        materiaModel.createIdea(title, htgs, content);
        setinAddDialog(false);
    }

    function onKeyPress(e) 
    {
        if(e.keyCode == 13)
        {
            setSearchText(e.target.value);
            updateSecondaryIdeaList(curIdea, e.target.value);
        }
    }

    function onIdeaListClick(id)
    {
        materiaModel.getIdea(id, (i)=>{
            setCurIdea(i);
            updateSecondaryIdeaList(i, searchText);
        });
    }

    return (
        <Grid container direction="row" justify="space-between" alignItems="flex-start">
            {inAddDialog && <IdeaEditorDialog onOk={onAddDialogOk} onCancel={onAddDialogCancel} object={{}}/>}
            <Backdrop open={updating}><CircularProgress color="inherit"/></Backdrop>
            <div style={{marginLeft: 20}}>
                <IconButton edge="start" onClick={onAddClicked}>
                    <AddCircleOutlineIcon/>
                </IconButton>
                <IconButton onClick={selectRandomIdea}>
                    <HelpOutlineIcon/>
                </IconButton>
            </div>
            <div style={{flexGrow: 1}} />
            <Box style={{width: '20vw', marginTop: 10}}>
                <div className={classes.search}>
                    <div className={classes.searchIcon}>
                        <SearchIcon />
                    </div>
                    <InputBase
                        placeholder="Enter query..."
                        classes={{
                            root: classes.inputRoot,
                            input: classes.inputInput,
                        }}
                        inputProps={{ 'aria-label': 'search', onKeyDown:onKeyPress }}
                    />
                </div>
                <List>
                {ideaList && ideaList.map(((obj, index) => {
                    return (
                        <ListItem button key={obj.id} onClick={() => onIdeaListClick(obj.id)}>
                            <ListItemIcon>
                                <EmojiObjectsIcon />
                            </ListItemIcon>
                            <ListItemText disableTypography primary={
                                <Typography variant="body1">
                                    {obj.title}
                                </Typography>}/>    
                        </ListItem>);
                    }))}
                </List>
            </Box>
            <Box style={{position: 'absolute', left: '50%', top: '50%', transform: 'translate(-50%, -50%)'}}>
                {curIdea && !updating && <IdeaView content={curIdea} onDeleted={onIdeaDeleted}/>}
            </Box>
        </Grid>
    );
}

export default IdeasView;