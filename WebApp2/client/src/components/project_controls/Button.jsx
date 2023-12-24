import React, {useState, useRef} from 'react';
import {
    Button
} from "@material-ui/core";

const PushButton = ({ control, state }) => {
    return (
        <Button variant="contained" style={{ width: control.w - 10, height: control.h - 24 }}>{control.value}</Button>
      );
};

export class PushButtonEditor extends React.Component {
    constructor(props) {
        super(props);
    }

    getContent() {
        const { control } = this.props;
        return {...control}
    }

    render() {
        const { control } = this.props;

        return (<div>soon</div>);
    }
}

export default PushButton;
