import React, {useState} from 'react';
import Static from './project_controls/Static.jsx'
import {StaticEditor} from './project_controls/Static.jsx'
import PushButton from './project_controls/Button.jsx'
import {PushButtonEditor} from './project_controls/Button.jsx'
import CheckboxM from './project_controls/Checkbox.jsx'
import {CheckboxEditor} from './project_controls/Checkbox.jsx'
import ObjectBinding from './project_controls/ObjectBinding.jsx'
import {ObjectBindingEditor} from './project_controls/ObjectBinding.jsx'
import TextBox from './project_controls/Textbox.jsx'
import {TextBoxEditor} from './project_controls/Textbox.jsx'

export const createControlEditor = (src) => {
    if(src.type === "static") {
        return (<StaticEditor control={src} />);
    } else if(src.type === "button") {
        return (<PushButtonEditor control={src} />);
    } else if(src.type === "checkbox") {
        return (<CheckboxEditor control={src} />);
    } else if(src.type === "object_binding") {
        return (<ObjectBindingEditor control={src} />);
    } else if(src.type === "textbox") {
        return (<TextBoxEditor control={src} />);
    } else {
        return (<div>error</div>);
    }
}

const createControl = (src, state, updateCb, stateUpdCb, projName, inputStore) => {
    if(src.type === "static") {
        return (<Static control={src} state={state}/>);
    } else if(src.type === "button") {
        return (<PushButton control={src} updateCb={updateCb} projName={projName} inputStore={inputStore}/>);
    } else if(src.type === "checkbox") {
        return (<CheckboxM control={src} state={state} stateUpdCb={stateUpdCb}/>);
    } else if(src.type === "object_binding") {
        return (<ObjectBinding control={src}/>);
    } else if(src.type === "textbox") {
        return (<TextBox control={src} inputStore={inputStore}/>);
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
