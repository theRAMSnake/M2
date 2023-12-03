import React, {useState} from 'react';
import Draggable from 'react-draggable';
import { Resizable } from 're-resizable';
import createControl from './ProjectControls.jsx'

const DesignerItem = ({ control }) => {
    const [width, setWidth] = useState(control.w);
    const [height, setHeight] = useState(control.h);
    const [position, setPosition] = useState({ x: control.x, y: control.y });

    const handleDrag = (e, data) => {
        setPosition({ x: data.x, y: data.y });
    };

return (
    <Draggable
      bounds="parent"
      handle=".drag-handle"
      position={position}
      onStop={handleDrag}
    >
      <Resizable size={{width: width, height: height}}
        onResizeStop={ ( event, direction, elt, delta ) => {
                setWidth(width + delta.width);
                setHeight(height + delta.height);
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
        <div style={{ border: '1px solid white', height: '100%', width: '100%' }}>
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
