
import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import {
    Grid,
    Toolbar,
    Paper,
    IconButton,
    Typography
} from "@material-ui/core";

const ProjectCollectionBar = ({ projName }) => {
    const [inNameDialog, setInNameDialog] = useState(false);
    const [myCollections, setMyCollections] = useState([]);

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

    return (
      <div>
      <Grid container direction="row" alignItems="flex-start">
          {inNameDialog && <TextQueryDialog text={""} onFinished={handleNameFinished} onCanceled={handleNameCanceled}/>}
          <Toolbar style={{ width: '100%' }} gap: '10px'>
              {myCollections.map((col) => (
                <Paper style={{ padding: '5px', display: 'flex', alignItems: 'center', cursor: 'pointer' }}>
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
