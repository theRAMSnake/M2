import React from 'react';
import Draggable from 'react-draggable';
import { ResizableBox } from 'react-resizable';

const DesignerItem = () => {
    return (
    <Draggable
      bounds="parent"
    >
      <ResizableBox width={200} height={200} minConstraints={[100, 100]} maxConstraints={[300, 300]}>
        <div style={{ border: '1px solid white', height: '100%', width: '100%' }}>
            Contents
        </div>
      </ResizableBox>
    </Draggable>
  );
};

export default DesignerItem;
