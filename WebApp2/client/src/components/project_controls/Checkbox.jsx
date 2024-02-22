import React, {useState, useRef} from 'react';
import {
    Button,
    Checkbox,
    FormControlLabel,
    TextField
} from "@material-ui/core";

const CheckboxM = ({ control, state, stateUpdCb }) => {
    return (
        <div>
            <FormControlLabel margin='dense' control={<Checkbox inputProps={{onChange: this.handleChange}} checked={state[control.binding]} />} label={control.label} />
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
