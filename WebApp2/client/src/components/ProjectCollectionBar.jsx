
import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import Grid from '@mui/material/Grid';
import IconButton from '@mui/material/IconButton';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'

const ProjectCollectionBar = ({ collections }) => {
    const [inNameDialog, setInNameDialog] = useState(false);
    const [myCollections, setMyCollections] = useState(collections);

    const handleAdd = () => {
        setInNameDialog(true);
    }

    const handleNameCanceled = () => {
        setInNameDialog(false);
    }

    const handleNameFinished = (name) => {
        setInNameDialog(false);
        //Update here
    }

    return (
      <Grid container direction="row" justify="space-around" alignItems="flex-start">
          {inNameDialog && <TextQueryDialog text={""} onFinished={handleNameFinished} onCanceled={handleNameCanceled}/>}
          <IconButton edge="start" aria-label="complete" onClick={() => handleAdd()}>
            <AddCircleOutlineIcon/>
          </IconButton>
      </Grid>
    );
};

export default ProjectCollectionBar;
