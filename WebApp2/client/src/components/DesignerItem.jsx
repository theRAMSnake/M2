import React, {useState} from 'react';
import Draggable from 'react-draggable';
import { Resizable } from 're-resizable';
import createControl from './ProjectControls.jsx'

const DesignerItem = ({ control, onControlChange }) => {
    const handleDrag = (e, data) => {
        control.x = data.x;
        control.y = data.y;
        onControlChange(control);
    };

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
        >
        <div style={{ border: '1px solid white', height: '100%', width: '100%', position:'absolute' }}>
            <div className="drag-handle" style={{ cursor: 'move', backgroundColor: 'lightblue', userSelect: 'none' }}>
              Drag here
            </div>
            {createControl(control)}
        </div>
      </Resizable>
    </Draggable>
  );
};

export default DesignerItem;
