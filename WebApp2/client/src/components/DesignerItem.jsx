import React from 'react';
import Draggable from 'react-draggable';
import { ResizableBox } from 're-resizable';

const DesignerItem = () => {
    return (
    <Draggable
      bounds="parent"
    >
      <ResizableBox width={200} height={200} minConstraints={[100, 100]} maxConstraints={[300, 300]}>
        <span>Contents</span>
      </ResizableBox>
    </Draggable>
  );
};

export default DesignerItem;
