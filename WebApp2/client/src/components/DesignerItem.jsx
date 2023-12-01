import React, {useState} from 'react';
import Draggable from 'react-draggable';
import { Resizable } from 're-resizable';

const DesignerItem = () => {
    const [width, setWidth] = useState(200);
    const [height, setHeight] = useState(200);

    return (
    <Draggable
      bounds="parent"
    >
      <Resizable size={{width: width, height: height}}
        onResizeStop={ ( event, direction, elt, delta ) => {
                setWidth(width + delta.width);
                setHeight(height + delta.height);
            } }
        >
        <div style={{ border: '1px solid white', height: '100%', width: '100%' }}>
            Contents
        </div>
      </Resizable>
    </Draggable>
  );
};

export default DesignerItem;
