import React, {useState} from 'react';
import createControl from './ProjectControls.jsx'

const ProjectItem = ({ control }) => {
    const [width, setWidth] = useState(control.w);
    const [height, setHeight] = useState(control.h);
    const [position, setPosition] = useState({ x: control.x, y: control.y });

return (
    <div style={{
            position: 'absolute', // Use absolute positioning
            left: `${control.x}px`, // Set left position
            top: `${control.y}px`, // Set top position
            width: `${control.w}px`, // Set width
            height: `${control.h}px`, // Set height
            border: '1px solid white'
        }}
    >
        {createControl(control)}
    </div>
  );
};

export default ProjectItem;
