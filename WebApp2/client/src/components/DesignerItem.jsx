import React, {useState} from 'react';
import Draggable from 'react-draggable';
import { ResizableBox } from 'react-resizable';

const DesignerItem = () => {
    const [width, setWidth] = useState(200);
    const [height, setHeight] = useState(200);

    return (
    <Draggable
      bounds="parent"
    >
      <ResizableBox width={width} height={height} minConstraints={[100, 100]} maxConstraints={[300, 300]}
        onResizeStop={ ( event, direction, elt, delta ) => {
                setWidth(width + delta.width);
                setHeight(height + delta.height);
                toggleSelection( true );
            } }
            onResizeStart={ () => {
                toggleSelection( false );
            } }
        >
        <div style={{ border: '1px solid white', height: '100%', width: '100%' }}>
            Contents
        </div>
      </ResizableBox>
    </Draggable>
  );
};

export default DesignerItem;
