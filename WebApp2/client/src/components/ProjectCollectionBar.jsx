
import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import {
    Grid,
    Toolbar,
    Paper,
    IconButton
} from "@material-ui/core";

const ProjectCollectionBar = ({ projName }) => {
    const [inNameDialog, setInNameDialog] = useState(false);
    const [myCollections, setMyCollections] = useState([]);

    useEffect(() => {
      ScriptHelper.exec("import projects\nprojects.project_collections('" + projName + "')", (data) => {
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
        ScriptHelper.exec("import projects\nprojects.bind_collection('" + projName + "', '" + name + "')\nresult=1");
    }

    return (
      <div>
      <Grid container direction="row" alignItems="flex-start">
          {inNameDialog && <TextQueryDialog text={""} onFinished={handleNameFinished} onCanceled={handleNameCanceled}/>}
          {myCollections.map((col) => (
            <Paper>
              {col}
            </Paper>
          ))}
          <Toolbar style={{ width: '100%' }}>
              <IconButton edge="start" onClick={() => handleAdd()}>
                <AddCircleOutlineIcon/>
              </IconButton>
          </Toolbar>
      </Grid>
      </div>
    );
};

export default ProjectCollectionBar;
