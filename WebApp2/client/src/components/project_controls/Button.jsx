import React, {useState, useRef} from 'react';
import {
    Button,
    Checkbox,
    FormControlLabel,
    TextField
} from "@material-ui/core";
import AceEditor from 'react-ace';
import 'ace-builds/src-noconflict/mode-json';
import 'ace-builds/src-noconflict/theme-monokai';

const PushButton = ({ control, state }) => {
    return (
        <Button variant="contained" style={{ width: control.w - 6, height: control.h - 48 }}>{control.value}</Button>
      );
};

export class PushButtonEditor extends React.Component {
    constructor(props) {
        super(props);
        const { control } = this.props;
        this.myObject = {...control};
    }

    getContent() {
        const { control } = this.props;
        return this.myObject;
    }

    handleCaptionChange = (e) => {
        this.myObject = {...this.myObject, caption: e.target.value};
    }

    handleConfirmationChange = (e) => {
        this.myObject = {...this.myObject, need_confirmation: String(e.target.checked)};
    }

    handleUpdateChange = (e) => {
        this.myObject = {...this.myObject, need_update: String(e.target.checked)};
    }

    render() {
        return(
            <div>
                <TextField inputProps={{onChange: this.handleCaptionChange}} value={this.myObject.value} fullWidth label="Caption" />;
                <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: this.handleConfirmationChange}} checked={this.myObject.need_confirmation === "true"} />} label="Need confirmation" />
                <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: this.handleUpdateChange}} checked={this.myObject.need_update === "true"} />} label="Need update" />
                <AceEditor
                  mode="python"
                  theme="monokai" // Choose your preferred theme
                  onChange={(newValue) => {this.myObject.script = newValue;}}
                  name="json-editor"
                  editorProps={{ $blockScrolling: true }}
                  value={this.myObject.script}
                  height="30vh"
                  showPrintMargin={false}
                  width="100%"
                  setOptions={{
                    tabSize: 2, // Adjust the tab size for indentation
                    useSoftTabs: true, // Use soft tabs (spaces) for indentation
                    wrap: true, // Enable line wrapping
                  }}
                />
            </div>
        );
    }
}

export default PushButton;
