import React, {useState} from 'react';
import Static from './project_controls/Static.jsx'
import {StaticEditor} from './project_controls/Static.jsx'
import Button from './project_controls/Button.jsx'
import {ButtonEditor} from './project_controls/Button.jsx'

export const createControlEditor = (src) => {
    if(src.type === "static") {
        return new StaticEditor({control: src});
    } else if(src.type === "button") {
        return new ButtonEditor({control: src});
    } else {
        return (<div>error</div>);
    }
}

const createControl = (src, state) => {
    if(src.type === "static") {
        return (<Static control={src} state={state}/>);
    } else if(src.type === "button") {
        return (<Button control={src}/>);
    } else {
        return (<div>error</div>);
    }
}

function generateRandomString(length) {
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    let result = '';
    const charactersLength = characters.length;

    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }

    return result;
}

export const newControl = (t) => {
    return {id: generateRandomString(10), type: t, value: "value", x: 0, y: 0, w: 100, h:100};
}

export default createControl;
