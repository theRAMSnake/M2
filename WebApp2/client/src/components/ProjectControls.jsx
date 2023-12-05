import React, {useState} from 'react';

const createControl = (src) => {
    if(src.type === "static") {
        return (<div>{src.value}</div>);
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

export const newControl = (src) => {
    return [{id: generateRandomString(10), type: "static", value: "value", x: 0, y: 0, w: 100, h:100}];
}

export default createControl;
