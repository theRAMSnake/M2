import React, {useState, useRef} from 'react';
import {
    Button,
    Checkbox,
    FormControlLabel,
    TextField
} from "@material-ui/core";

const CheckboxM = ({ control, state, stateUpdCb }) => {
    let checked = false;
    if(state && control.binding && state[control.binding]) {
        checked = state[control.binding] === "true";
    }
    const label = control.label ? control.label : "";

    const handleChange = (e) => {
        let newState = {...state};
        newState[control.binding] = e.target.checked ? "true" : "false";
        stateUpdCb(newState);
    }

    return (
        <div>
            <FormControlLabel margin='dense' control={<Checkbox inputProps={{onChange: handleChange}} checked={checked} />} label={label} />
        </div>
      );
};

export class CheckboxEditor extends React.Component {
    constructor(props) {
        super(props);
        const { control } = this.props;
        this.state = {
            myObject: {...control}
        }
    }

    getContent() {
        const { myObject } = this.state;
        return myObject;
    }

    handleLabelChange = (e) => {
        this.setState(prevState => ({
            myObject : {...prevState.myObject, label: e.target.value}
        }));
    }

    handleBindingChange = (e) => {
        this.setState(prevState => ({
            myObject : {...prevState.myObject, binding: e.target.value}
        }));
    }

    render() {
        const { myObject } = this.state;
        return(
            <div>
                <TextField inputProps={{onChange: this.handleLabelChange}} value={myObject.label} fullWidth label="Label" />
                <TextField inputProps={{onChange: this.handleBindingChange}} value={myObject.binding} fullWidth label="Binding" />
            </div>
        );
    }
}

export default CheckboxM;
