import React, {useState} from 'react';

const createControl = (src) => {
    if(src.type === "static") {
        return (<div>{src.value}<div>);
    } else {
        return (<div>value<div>);
    }
}

export default createControl;
