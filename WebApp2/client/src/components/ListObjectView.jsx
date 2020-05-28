import React, { useState } from 'react';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import SimpleDialog from './dialogs/SimpleDialog.jsx'
import {
    Dialog,
    IconButton,
    AppBar,
    Toolbar,
    Typography,
    List,
    ListItem,
    ListItemText,
    ListItemSecondaryAction,
    Divider
} from "@material-ui/core";
import CloseIcon from '@material-ui/icons/Close';
import ListIcon from '@material-ui/icons/List';
import DeleteIcon from '@material-ui/icons/Delete';
import ClearAllIcon from '@material-ui/icons/ClearAll';
import HelpOutlineIcon from '@material-ui/icons/HelpOutline';
import AddCircleOutlineIcon from '@material-ui/icons/AddCircleOutline';
import Slide from '@material-ui/core/Slide';
import { makeStyles, createStyles } from '@material-ui/core/styles';

const useStyles = makeStyles((theme) =>
  createStyles({
    appBar: {
      position: 'relative',
      color: 'inherit',
      'background-color': '#424242'
    },
    grow: {
        flexGrow: 1
    },
    title: {
        display: 'none',
        color: 'white',
        [theme.breakpoints.up('sm')]: {
            display: 'block',
        },
    },
  }),
);

const Transition = React.forwardRef(function Transition(props, ref) 
{
    return <Slide direction="up" ref={ref} {...props} />;
});

export default function ListObjectView(props) 
{
    const classes = useStyles();

    const [textQueryVisible, setTextQueryVisible] = useState(false);
    const [insertVisible, setInsertVisible] = useState(false);
    const [randomVisible, setRandomVisible] = useState(false);
    const [randomItemText, setRandomItemText] = useState('');
    const [inDeleteDialog, setInDeleteDialog] = useState(false);
    const [inClearDialog, setInClearDialog] = useState(false);
    const [focusedItemIndex, setFocusedItemIndex] = useState(-1);
    const object = props.object;

    function prepareAdd()
    {
        setTextQueryVisible(true);
    }

    function prepareInsert()
    {
        setInsertVisible(true);
    }

    function addItem(newItem)
    {
        if(!object.items)
        {
            var items = [{
                text: newItem
            }];

            object['items'] = items;            
        }
        else
        {
            object['items'].push({
                text: newItem
            });
        }

        props.onChange(JSON.parse(JSON.stringify(object)));
    }

    function modifyItem(newItem)
    {
        object['items'][focusedItemIndex] = {
            text: newItem
        };

        props.onChange(JSON.parse(JSON.stringify(object)));

        setFocusedItemIndex(-1);
    }

    function handleTextQueryFinished(text)
    {
        setTextQueryVisible(false);
        if(focusedItemIndex == -1)
        {
            addItem(text);
        }
        else
        {
            modifyItem(text);
        }
    }

    function handleTextQueryCanceled()
    {
        setTextQueryVisible(false);
        setFocusedItemIndex(-1);
    }

    function prepareDelete(index)
    {
        setInDeleteDialog(true);
        setFocusedItemIndex(index);
    }

    function prepareModify(index)
    {
        setTextQueryVisible(true);
        setFocusedItemIndex(index);
    }

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
        setFocusedItemIndex(-1);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);

        object['items'].splice(focusedItemIndex, 1);

        props.onChange(JSON.parse(JSON.stringify(object)));

        setFocusedItemIndex(-1);
    }

    function getInitialText()
    {
        if(focusedItemIndex == -1)
        {
            return '';
        }
        else
        {
            return object['items'][focusedItemIndex].text;
        }
    }

    function handleInsertFinished(text)
    {
        setInsertVisible(false);

        if(!object.items)
        {
            var items = [];
            object['items'] = items;            
        }

        var array = text.split('\n');
        var i = 0;
        for(i = 0; i < array.length; ++i)
        {
            object['items'].push({
                text: array[i]
            });
        }

        props.onChange(JSON.parse(JSON.stringify(object)));
    }

    function handleInsertCanceled()
    {
        setInsertVisible(false);
    }

    function getRandomItem()
    {
        setRandomVisible(true);
        setRandomItemText(getRandomItemText());
    }

    function hideRandom()
    {
        setRandomVisible(false);
    }

    function getRandomItemText()
    {
        if(object.items)
        {
            return object.items[Math.floor(Math.random() * object.items.length)].text;
        }

        return '';
    }

    function onClearCancel()
    {
        setInClearDialog(false);
    }

    function onClearOk()
    {
        setInClearDialog(false);

        var items = [];
        object['items'] = items;

        props.onChange(JSON.parse(JSON.stringify(object)));
    }

    function prepareClearAll()
    {
        setInClearDialog(true);
    }

    return ( 
        <Dialog open={props.open} onClose={props.onClose} fullScreen TransitionComponent={Transition}>
            <AppBar className={classes.appBar}>
                <Toolbar>
                    <IconButton onClick={prepareAdd}>
                        <AddCircleOutlineIcon/>
                    </IconButton>
                    <IconButton onClick={prepareInsert}>
                        <ListIcon/>
                    </IconButton>
                    <IconButton onClick={getRandomItem}>
                        <HelpOutlineIcon/>
                    </IconButton>
                    <IconButton onClick={prepareClearAll}>
                        <ClearAllIcon/>
                    </IconButton>
                    <Typography variant="h6" className={classes.title}>
                        {object.id}
                    </Typography>
                    <div className={classes.grow} />
                    <IconButton edge="end" color="inherit" onClick={props.onClose} aria-label="close">
                        <CloseIcon />
                    </IconButton>
                </Toolbar>
            </AppBar>
            {object.items &&
            <List>
                {object.items.map((item, index) =>
                    <div>
                        <ListItem button onClick={event => prepareModify(index)}>
                            <ListItemText primary={item.text} />
                            <ListItemSecondaryAction>
                                <IconButton edge="end" aria-label="delete" onClick={() => prepareDelete(index)}>
                                    <DeleteIcon />
                                </IconButton>
                            </ListItemSecondaryAction>
                        </ListItem>
                        <Divider />
                    </div>
                )}
            </List>}
            <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
            <ConfirmationDialog open={inClearDialog} question="clear all" caption="confirm clear all" onNo={onClearCancel} onYes={onClearOk} />
            <SimpleDialog open={randomVisible} onClose={hideRandom} text={randomItemText}/>
            {textQueryVisible && <TextQueryDialog text={getInitialText()} onFinished={handleTextQueryFinished} onCanceled={handleTextQueryCanceled}/>}
            {insertVisible && <TextQueryDialog text={''} onFinished={handleInsertFinished} onCanceled={handleInsertCanceled}/>}
        </Dialog>
    );
}