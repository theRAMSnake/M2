
import React, { useState } from 'react';
import ScriptHelper from '../modules/script_helper'
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'
import {
    Grid,
    IconButton
} from "@material-ui/core";

const ProjectCollectionBar = ({ projName }) => {
    const [inNameDialog, setInNameDialog] = useState(false);
    const [myCollections, setMyCollections] = useState([]);

    const handleAdd = () => {
        setInNameDialog(true);
    }

    const handleNameCanceled = () => {
        setInNameDialog(false);
    }

    const handleNameFinished = (name) => {
        setInNameDialog(false);
        ScriptHelper.exec("import projects\nprojects.bind_collection(" + projName + ", " + name + ")\nresult=1");
    }

    return (
      <div>
      <Grid container direction="row" justify="space-around" alignItems="flex-start">
          {inNameDialog && <TextQueryDialog text={""} onFinished={handleNameFinished} onCanceled={handleNameCanceled}/>}
          <div>
              <IconButton edge="start" onClick={() => handleAdd()}>
                <AddCircleOutlineIcon/>
              </IconButton>
          </div>
      </Grid>
      </div>
    );
};

export default ProjectCollectionBar;
