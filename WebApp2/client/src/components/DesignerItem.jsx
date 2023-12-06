import React, {useState} from 'react';
import Draggable from 'react-draggable';
import { Resizable } from 're-resizable';
import createControl from './ProjectControls.jsx'

const DesignerItem = ({ control, onControlChange }) => {
    const handleDrag = (e, data) => {
        const snappedX = Math.round(data.x / 10) * 10;
        const snappedY = Math.round(data.y / 10) * 10;
        control.x = snappedX;
        control.y = snappedY;
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
