import React, {useState} from 'react';

const Static = ({ control }) => {
    return (
        <div>
            {control.value}
        </div>
      );
};

export const StaticEditor = ({ control, changeCb }) => {
    return (
        <div>
            {control.value}
        </div>
      );
};

export default Static;
