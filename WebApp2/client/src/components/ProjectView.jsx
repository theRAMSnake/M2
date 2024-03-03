import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import ProjectCollectionBar from './ProjectCollectionBar.jsx'
import DesignerItem from './DesignerItem.jsx'
import Materia from '../modules/materia_request'
import ProjectItem from './ProjectItem.jsx'
import {
    Toolbar,
    IconButton,
    Divider,
    Dialog,
    DialogActions,
    DialogContent,
    DialogContentText,
    Button,
    Checkbox
} from "@material-ui/core";
import EditIcon from '@material-ui/icons/Edit';
import UpdateIcon from '@material-ui/icons/Update';
import FontDownloadIcon from '@material-ui/icons/FontDownload';
import Crop169Icon from '@material-ui/icons/Crop169';
import CheckIcon from '@material-ui/icons/Check';
import ExplicitIcon from '@material-ui/icons/Explicit';
import CommitIcon from '@material-ui/icons/Commit';
import EditorDialog from './dialogs/EditorDialog.jsx'
import { newControl } from './ProjectControls.jsx'

const testControls = () => {
    return [{id: "5", type: "static", value: "value", x: 0, y: 0, w: 100, h:100}];
}

const ProjectView = ({ projName, projectId }) => {
    const [updateScript, setUpdateScript] = useState(null);
    const [inScriptEditDialog, setInScriptEditDialog] = useState(false);
    const [inStateEditDialog, setInStateEditDialog] = useState(false);
    const [designerMode, setDesignerMode] = useState(false);
    const [controls, setControls] = useState([]);
    const [showUpdateResult, setShowUpdateResult] = useState(false);
    const [updateResult, setUpdateResult] = useState("");
    const [state, setState] = useState(null);
    const [version, setVersion] = useState(0);

    useEffect(() => {
      const req = {
          operation: "query",
          filter: `ChildOf("${projectId}") AND .elementType = "control"`
      };

      Materia.exec(req, (r) =>
      {
          let newControls = [];
          for (let i = 0; i < r.object_list.length; i++) {
              let newControl = {...r.object_list[i]};
              newControl.x = parseInt(newControl.x, 10);
              newControl.y = parseInt(newControl.y, 10);
              newControl.w = parseInt(newControl.w, 10);
              newControl.h = parseInt(newControl.h, 10);
              newControls.push(newControl);
          }
          setControls(newControls);
      });

      const req2 = {
          operation: "query",
          filter: `ChildOf("${projectId}") AND .elementType = "state"`
      };

      Materia.exec(req2, (r) =>
      {
          setState(r.object_list[0]);
      });
    }, [projectId, version]);

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

    const handleEditState = () => {
        setInStateEditDialog(true);
    }

    const handleCloseDialog = () => {
        setInScriptEditDialog(false);
        setInStateEditDialog(false);
    }

    const handleUpdate = () => {
        ScriptHelper.exec_string_or_error(`import projects\nprojects.update_project('${projName}')\nresult=1`, (data)=>{
            setShowUpdateResult(true);
            setUpdateResult(data);
            if(data === '1') {
                setVersion(version + 1);
            }
        });
    }

    const handleUpdateSilent = () => {
        ScriptHelper.exec_string_or_error(`import projects\nprojects.update_project('${projName}')\nresult=1`, (data)=>{
            if(data === '1') {
                setVersion(version + 1);
            }
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

    const handleStateSave = (newState) => {
       let obj = JSON.parse(newState)
       setState(obj)
       Materia.postEdit(obj.id, JSON.stringify(obj));
       setInStateEditDialog(false);
    }

    const handleStateSaveFromObject = (newState) => {
       setState(newState)
       Materia.postEdit(newState.id, JSON.stringify(newState));
       setInStateEditDialog(false);
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
    };

    const onControlChange = (c) => {
        let newControls = [];

        for (let i = 0; i < controls.length; i++) {
            if(controls[i].id === c.id) {
                if(c.deleteFlag) {
                    Materia.postDelete(c.id);
                    continue;
                } else {
                    controls[i] = JSON.parse(JSON.stringify(c));
                    Materia.postEdit(c.id, JSON.stringify(c));
                }
            }
            newControls.push(controls[i]);
        }
        setControls(newControls);
    };

    const MessageBox = ({ message, onClose }) => {
        return (
            <Dialog
                open={true}
                onClose={onClose}
                aria-labelledby="alert-dialog-title"
                aria-describedby="alert-dialog-description"
            >
                <DialogContent>
                    <DialogContentText id="alert-dialog-description">
                        {updateResult}
                    </DialogContentText>
                </DialogContent>
                <DialogActions>
                    <Button onClick={onClose} color="primary" autoFocus>
                        Close
                    </Button>
                </DialogActions>
            </Dialog>
        );
    };

    const handleNewControl = (c) => {
        let script = `import projects\nimport m4\n${ScriptHelper.jsonToM4O("a", JSON.stringify(c))}\nresult = projects.create_project_control("${projName}", a)`;
        ScriptHelper.exec_string(script, (data)=>{
            c.id = data;
            controls.push(c);
            setControls(controls.slice());
        });
    }

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
           <IconButton edge="start" onClick={() => handleEditState()}>
             <ExplicitIcon/>
           </IconButton>
           <Checkbox checked={designerMode} onChange={handleCheckboxChange}/>
           {designerMode && <Divider/>}
           {designerMode &&
               <IconButton edge="start" onClick={() => handleNewControl(newControl("static"))}>
                 <FontDownloadIcon/>
               </IconButton>}
           {designerMode &&
               <IconButton edge="start" onClick={() => handleNewControl(newControl("button"))}>
                 <Crop169Icon/>
               </IconButton>}
           {designerMode &&
               <IconButton edge="start" onClick={() => handleNewControl(newControl("checkbox"))}>
                 <CheckIcon/>
               </IconButton>}
           {designerMode &&
               <IconButton edge="start" onClick={() => handleNewControl(newControl("object_binding"))}>
                 <CommitIcon/>
               </IconButton>}
        </Toolbar>
        {inScriptEditDialog && <EditorDialog onClose={handleCloseDialog} text={updateScript} onSave={handleScriptSave} mode="python" />}
        {inStateEditDialog && <EditorDialog onClose={handleCloseDialog} text={JSON.stringify(state, null, 2)} onSave={handleStateSave} mode="json" />}
        <div style={containerStyle}>
            {controls.map((c) => (
                designerMode ?
                    <DesignerItem control={c} onControlChange={(x) => onControlChange(x)} state={state} updateCb={() => {}} /> :
                    <ProjectItem control={c} state={state} updateCb={handleUpdateSilent} stateUpdCb={handleStateSaveFromObject} projName={projName} />
            ))}
        </div>
        </div>
    );
};

export default ProjectView;
