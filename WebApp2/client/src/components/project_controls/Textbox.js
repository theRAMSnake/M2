import React, {useState, useRef} from 'react';
import {
    Button,
    FormControlLabel,
    TextField
} from "@material-ui/core";

const Textbox = ({ control, state, stateUpdCb }) => {
    const [value, setValue] = useState("");
    const handleChange = (e) => {
        setValue(e.target.value)
    }

    return (
        <div>
            <FormControlLabel style={{ marginLeft: '10px' }} margin='dense' control={<TextField inputProps={{onChange: handleChange}} value={value} />} />
        </div>
      );
};

export class TextBoxEditor extends React.Component {
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

export default Textbox;
