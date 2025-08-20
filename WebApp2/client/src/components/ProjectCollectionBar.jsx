
import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import Slide from '@material-ui/core/Slide';
import {
    Grid,
    Toolbar,
    Dialog,
    Paper,
    IconButton,
    Typography
} from "@material-ui/core";
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import CollectionView from './CollectionView.jsx'

const Transition = React.forwardRef(function Transition(props, ref)
{
    return <Slide direction="up" ref={ref} {...props} />;
});

const ProjectCollectionBar = ({ projName }) => {
    const [inNameDialog, setInNameDialog] = useState(false);
    const [inDeleteDialog, setInDeleteDialog] = useState(false);
    const [colViewOpen, setColViewOpen] = useState(false);
    const [myCollections, setMyCollections] = useState([]);
    const [selected, setSelected] = useState("");

    useEffect(() => {
      ScriptHelper.exec("import projects\nresult = projects.project_collections('" + projName + "')", (data) => {
          setMyCollections(data);
      });
    }, [projName]); // re-run the effect if colName changes

    const handleAdd = () => {
        setInNameDialog(true);
    }

    const handleNameCanceled = () => {
        setInNameDialog(false);
    }

    const handleNameFinished = (name) => {
        setInNameDialog(false);
        ScriptHelper.exec("import projects\nprojects.bind_collection('" + projName + "', '" + name + "')\nresult=1", (data)=>{});
    }

    const handleLeftClick = (colName) => {
        setSelected(colName);
        setColViewOpen(true);
    };

    const handleRightClick = (event, colName) => {
        event.preventDefault(); // Prevents the default context menu
        setInDeleteDialog(true);
        setSelected(colName);
    };

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        const index = myCollections.findIndex(col => col === selected);
        if (index > -1) {
            const updatedCollections = [...myCollections]; // Create a copy of the array
            updatedCollections.splice(index, 1); // Remove the item at the found index
            setMyCollections(updatedCollections); // Update the state with the new array
            ScriptHelper.exec("import projects\nprojects.unbind_collection('" + projName + "', '" + selected + "')\nresult=1", (data)=>{});
        }
    }

    function onColViewClose() {
        setColViewOpen(false);
    }

    return (
      <div>
      <Grid container direction="row" alignItems="flex-start">
          {inNameDialog && <TextQueryDialog text={""} onFinished={handleNameFinished} onCanceled={handleNameCanceled}/>}
          <ConfirmationDialog open={inDeleteDialog} question="delete" caption="confirm delete" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
          <Dialog open={colViewOpen} onClose={onColViewClose} fullScreen TransitionComponent={Transition}>
              <CollectionView colName={selected}/>
          </Dialog>
          <Toolbar style={{ width: '100%', gap: '10px'}} >
              {myCollections.map((col) => (
                <Paper style={{ padding: '5px', display: 'flex', alignItems: 'center', cursor: 'pointer' }}
                       onClick={() => handleLeftClick(col)}
                       onContextMenu={(event) => handleRightClick(event, col)}>
                  <Typography variant="h6" elevation={3}>{col}</Typography>
                </Paper>
              ))}
              <IconButton edge="start" onClick={() => handleAdd()}>
                <AddCircleOutlineIcon/>
              </IconButton>
          </Toolbar>
      </Grid>
      </div>
    );
};

export default ProjectCollectionBar;
