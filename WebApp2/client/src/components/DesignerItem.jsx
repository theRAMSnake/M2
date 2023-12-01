import React from 'react';
import Draggable from 'react-draggable';
import { Resizable } from 're-resizable';

const DesignerItem = () => {
    return (
    <Draggable
      bounds="parent"
    >
      <Resizable
        defaultSize={{
          width: 200,
          height: 200,
        }}
        style={{ border: '1px solid black' }} // Add custom styles
      >
        <div className="drag-handle" style={{ cursor: 'move', backgroundColor: 'lightblue', userSelect: 'none' }}>
          Drag here
        </div>
        <div>
          Your content here
        </div>
      </Resizable>
    </Draggable>
  );
};

export default DesignerItem;
