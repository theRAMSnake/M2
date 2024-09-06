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
import ConfirmationDialog from '../dialogs/ConfirmationDialog.jsx'
import ScriptHelper from '../../modules/script_helper'

const PushButton = ({ control, updateCb, projName, inputStore }) => {
    const [inConfirmationDialog, setInConfirmationDialog] = useState(false);

    function onConfirmationDialogCancel()
    {
        setInConfirmationDialog(false);
    }

    function onConfirmationDialogOk()
    {
        setInConfirmationDialog(false);
        action();
        updateCb();
    }

    const onClick = () => {
        if(control.need_confirmation === 'true') {
            setInConfirmationDialog(true);
        } else {
            action();
            updateCb();
        }
    }

    function escapeForPython(str) {
        // Replace backslash with double backslash and double quotes with escaped double quotes
        return str.replace(/\\/g, '\\\\').replace(/"/g, '\\"');
    }

    function replaceInputTokens(str, map) {
        // Use a regular expression to find all occurrences of tokens with double underscores
        return str.replace(/__([a-zA-Z0-9_]+)/g, function(match, key) {
            // Check if the key exists in the map and replace with its value, otherwise keep the original match
            return key in map ? map[key] : match;
        });
    }

    const action = () => {
        let preResultScript = replaceInputTokens(control.script, inputStore)
        let resultScript = escapeForPython(preResultScript);
        ScriptHelper.exec(`import projects\nscript="""${resultScript}"""\nprojects.run_project_script("${projName}", script)\nresult=1`, (data)=>{
            console.log(data)
        });
    }

    return (
        <div>
            <ConfirmationDialog open={inConfirmationDialog} question="confirmation" caption="are you sure?" onNo={onConfirmationDialogCancel} onYes={onConfirmationDialogOk} />
            <Button
                variant="contained"
                style={{ width: control.w - 6, height: control.h - 48 }}
                onClick={() => onClick()}
            >{control.value}</Button>
        </div>
      );
};

export class PushButtonEditor extends React.Component {
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

    handleCaptionChange = (e) => {
        this.setState(prevState => ({
            myObject : {...prevState.myObject, value: e.target.value}
        }));
    }

    handleConfirmationChange = (e) => {
        this.setState(prevState => ({
            myObject : {...prevState.myObject, need_confirmation: e.target.checked ? "true" : "false"}
        }));
    }

    handleUpdateChange = (e) => {
        this.setState(prevState => ({
            myObject : {...prevState.myObject, need_update: e.target.checked ? "true" : "false"}
        }));
    }

    render() {
        const { myObject } = this.state;
        return(
            <div>
                <TextField inputProps={{onChange: this.handleCaptionChange}} value={myObject.value} fullWidth label="Caption" />
            <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: this.handleConfirmationChange}} checked={myObject.need_confirmation === "true"} />} label="Need confirmation" />
            <FormControlLabel margin='dense' fullWidth control={<Checkbox inputProps={{onChange: this.handleUpdateChange}} checked={myObject.need_update === "true"} />} label="Need update" />
                <AceEditor
                  mode="python"
                  theme="monokai" // Choose your preferred theme
                  onChange={(newValue) => {myObject.script = newValue;}}
                  name="json-editor"
                  editorProps={{ $blockScrolling: true }}
                  value={myObject.script}
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
