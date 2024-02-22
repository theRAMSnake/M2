import React, {useState, useRef} from 'react';
import Draggable from 'react-draggable';
import { Resizable } from 're-resizable';
import createControl, {createControlEditor} from './ProjectControls.jsx'
import {
    IconButton,
    Dialog,
    DialogActions,
    DialogContent,
    TextField,
    Button
} from "@material-ui/core";
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import EditIcon from '@material-ui/icons/Edit';
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'

const DesignerItemEditor = ({ content, onCanceled, onFinished }) => {
    const contentRef = useRef();
    return <Dialog fullWidth onClose={onCanceled} open={true}>
        <DialogContent>
           {React.cloneElement(content, { ref: contentRef })}
        </DialogContent>
        <DialogActions>
            <Button variant="contained" onClick={onCanceled} color="primary">
                Cancel
            </Button>
            <Button variant="contained" onClick={() => onFinished(contentRef.current.getContent())} color="primary" >
                Ok
            </Button>
        </DialogActions>
    </Dialog>
}

const DesignerItem = ({ control, onControlChange, state }) => {
    const [inDeleteDialog, setInDeleteDialog] = useState(false);
    const [inModifyDialog, setInModifyDialog] = useState(false);
    const [editor, setEditor] = useState(null);

    const handleDrag = (e, data) => {
        const snappedX = Math.round(data.x / 10) * 10;
        const snappedY = Math.round(data.y / 10) * 10;
        control.x = snappedX;
        control.y = snappedY;
        onControlChange(control);
    };

    const handleDelete = () => {
        setInDeleteDialog(true);
    }

    const handleModify = () => {
        setEditor(createControlEditor(control))
        setInModifyDialog(true);
    }

    const onEditorCanceled = () => {
        setInModifyDialog(false);
    }

    const onEditorFinished = (newControl) => {
        onControlChange(newControl);
        setInModifyDialog(false);
    }

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        control.deleteFlag = true;
        onControlChange(control);
    }

return (
    <Draggable
      bounds="parent"
      handle=".drag-handle"
      position={{x: control.x, y: control.y}}
      onStop={handleDrag}
    >
      <Resizable size={{width: control.w, height: control.h}}
        onResizeStop={ ( event, direction, elt, delta ) => {
                control.w = control.w + delta.width;
                control.h = control.h + delta.height;
                control.w = Math.round(control.w / 10) * 10;
                control.h = Math.round(control.h / 10) * 10;
                onControlChange(control);
            } }
        enable={{
                    top: false,
                    right: true,
                    bottom: true,
                    left: false,
                    topRight: false,
                    bottomRight: true,
                    bottomLeft: false,
                    topLeft: false
                }}
        style={{position: 'absolute'}}
        minWidth={50}
        minHeight={25}
        maxWidth={500}
        maxHeight={500}
        >
        <div style={{ border: '1px solid white', height: '100%', width: '100%', position:'absolute' }}>
            <ConfirmationDialog open={inDeleteDialog} question="delete" caption="confirm delete" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
            <div className="drag-handle" style={{ cursor: 'move', backgroundColor: 'lightblue', userSelect: 'none' }}>
              Drag here
            </div>
            <IconButton size="small" style={{ position: 'absolute', bottom: 0, left: 0 }} onClick={() => handleDelete()}>
              <DeleteForeverIcon fontSize="inherit"/>
            </IconButton>
            <IconButton size="small" style={{ position: 'absolute', bottom: 0, right: 0 }} onClick={() => handleModify()}>
              <EditIcon fontSize="inherit"/>
            </IconButton>
            {createControl(control, state, () => {}, () => {}), ""}
            {inModifyDialog && <DesignerItemEditor content={editor} onCanceled={onEditorCanceled} onFinished={onEditorFinished}/>}
        </div>
      </Resizable>
    </Draggable>
  );
};

export default DesignerItem;
