import React, {useState} from 'react';
import createControl from './ProjectControls.jsx'

const ProjectItem = ({ control }) => {
    const [width, setWidth] = useState(control.width);
    const [height, setHeight] = useState(control.height);
    const [position, setPosition] = useState({ x: control.x, y: control.y });

return (
    <div style={{
            position: 'absolute', // Use absolute positioning
            left: `${position.x}px`, // Set left position
            top: `${position.y}px`, // Set top position
            width: `${width}px`, // Set width
            height: `${height}px`, // Set height
            border: '1px solid white'
        }}
    >
        {createControl(c)}
    </div>
  );
};

export default ProjectItem;