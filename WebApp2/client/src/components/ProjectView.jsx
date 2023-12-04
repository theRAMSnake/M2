import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import ProjectCollectionBar from './ProjectCollectionBar.jsx'
import DesignerItem from './DesignerItem.jsx'
import ProjectItem from './ProjectItem.jsx'
import {
    Toolbar,
    IconButton,
    Checkbox
} from "@material-ui/core";
import EditIcon from '@material-ui/icons/Edit';
import UpdateIcon from '@material-ui/icons/Update';
import EditorDialog from './dialogs/EditorDialog.jsx'

const testControls = () => {
    return [{id: "5", type: "static", value: "value", x: 0, y: 0, w: 100, h:100}];
}

const ProjectView = ({ projName }) => {
    const [updateScript, setUpdateScript] = useState(null);
    const [inScriptEditDialog, setInScriptEditDialog] = useState(false);
    const [designerMode, setDesignerMode] = useState(true);
    const [controls, setControls] = useState(testControls());
    const [showUpdateResult, setShowUpdateResult] = useState(false);
    const [updateResult, setUpdateResult] = useState("");

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
        ScriptHelper.exec_string_or_error(`import projects\nprojects.update_project('${projName}')\nresult=1`, (data)=>{
            setShowUpdateResult(true);
            setUpdateResult(data);
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
        width: '98vw',  // 100% of the viewport width
        height: '75vh',  // 75% of the viewport height
        border: '1px solid white', // Optional, for visualizing the container
        boxSizing: 'border-box', // Ensures that the border is included in the width/height
        position: 'relative'
    };

    const handleCheckboxChange = (event) => {
        setDesignerMode(event.target.checked);
        /*if(!event.target.checked) {
            for (let i = 0; i < controls.length; i++) {
                controls[i] = JSON.parse(JSON.stringify(c));
            }
            setControls(...controls);
        }*/
    };

    const onControlChange = (c) => {
        for (let i = 0; i < controls.length; i++) {
            if(controls[i].id === c.id) {
                controls[i] = JSON.parse(JSON.stringify(c));
            }
        }
        setControls(...controls);
    };

    const MessageBox = ({ message, onClose }) => {
        return (
            <div className="message-box">
                <p>{message}</p>
                <button onClick={onClose}>Close</button>
            </div>
        );
    };

    return (
        <div>
        {showUpdateResult && (
            <MessageBox
                message={updateResult}
                onClose={() => setShowUpdateResult(false)}
            />
        )}
        <ProjectCollectionBar projName={projName}/>
        <Toolbar style={{ width: '100%', gap: '10px'}} >
           <IconButton edge="start" onClick={() => handleEdit()}>
             <EditIcon/>
           </IconButton>
           <IconButton edge="start" onClick={() => handleUpdate()}>
             <UpdateIcon/>
           </IconButton>
           <Checkbox checked={designerMode} onChange={handleCheckboxChange}/>
        </Toolbar>
        {inScriptEditDialog && <EditorDialog onClose={handleCloseDialog} text={updateScript} onSave={handleScriptSave} mode="python" />}
        <div style={containerStyle}>
            {controls.map((c) => (
                designerMode ?
                    <DesignerItem control={c} onChange={(x) => onControlChange(x)} /> :
                    <ProjectItem control={c} />
            ))}
        </div>
        </div>
    );
};

export default ProjectView;
