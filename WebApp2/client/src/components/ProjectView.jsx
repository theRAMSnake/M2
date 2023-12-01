import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import ProjectCollectionBar from './ProjectCollectionBar.jsx'
import DesignerItem from './DesignerItem.jsx'
import {
    Toolbar,
    IconButton
} from "@material-ui/core";
import EditIcon from '@material-ui/icons/Edit';
import UpdateIcon from '@material-ui/icons/Update';
import EditorDialog from './dialogs/EditorDialog.jsx'

const ProjectView = ({ projName }) => {
    const [updateScript, setUpdateScript] = useState(null);
    const [inScriptEditDialog, setInScriptEditDialog] = useState(false);
    const [designerMode, setDesignerMode] = useState(true);

    const handleEdit = () => {
        if(!updateScript) {
            ScriptHelper.exec_string("import projects\nresult=projects.project_update_script('" + projName + "')", (data)=>{
                setUpdateScript(data);
                setInScriptEditDialog(true);
            });
        } else {
            setInScriptEditDialog(true);
        }
    }

    const handleCloseDialog = () => {
        setInScriptEditDialog(false);
    }

    const handleUpdate = () => {
        ScriptHelper.exec(`import projects\nprojects.update_project('${projName}')\nresult=1`, (data)=>{
        });
    }

    function escapeForPython(str) {
        // Replace backslash with double backslash and double quotes with escaped double quotes
        return str.replace(/\\/g, '\\\\').replace(/"/g, '\\"');
    }

    const handleScriptSave = (newScript) => {
        let resultScript = escapeForPython(newScript);
        ScriptHelper.exec(`import projects\nscript="""${resultScript}"""\nprojects.modify_project_update_script("${projName}", script)\nresult=1`, (data)=>{
            setUpdateScript(newScript);
            setInScriptEditDialog(false);
        });
    }

    const containerStyle = {
        width: '100vw',  // 100% of the viewport width
        height: '75vh',  // 75% of the viewport height
        border: '1px solid black', // Optional, for visualizing the container
        boxSizing: 'border-box' // Ensures that the border is included in the width/height
    };

    return (
        <div>
        <ProjectCollectionBar projName={projName}/>
        <Toolbar style={{ width: '100%', gap: '10px'}} >
           <IconButton edge="start" onClick={() => handleEdit()}>
             <EditIcon/>
           </IconButton>
           <IconButton edge="start" onClick={() => handleUpdate()}>
             <UpdateIcon/>
           </IconButton>
        </Toolbar>
        {inScriptEditDialog && <EditorDialog onClose={handleCloseDialog} text={updateScript} onSave={handleScriptSave} mode="python" />}
        <div style={containerStyle}>
            {designerMode && <DesignerItem/>}
        </div>
        </div>
    );
};

export default ProjectView;
