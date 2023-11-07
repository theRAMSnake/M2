import React, { useState, useEffect } from 'react';
import ScriptHelper from '../modules/script_helper'
import ProjectCollectionBar from './ProjectCollectionBar.jsx'

const ProjectView = ({ projName }) => {
    return (
        <ProjectCollectionBar projName={projName}/>
    );
};

export default ProjectView;
