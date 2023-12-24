import React, {useState, useRef} from 'react';
import {
    Button
} from "@material-ui/core";

const Button = ({ control, state }) => {
    return (
        <Button variant="contained">{control.value}</Button>
      );
};

export class ButtonEditor extends React.Component {
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

export default Button;
