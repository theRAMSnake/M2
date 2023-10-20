import React, { useState } from 'react';
import Materia from '../modules/materia_request'

import { AceEditor } from "react-ace";
import "ace-builds/src-noconflict/mode-python";
import "ace-builds/src-noconflict/theme-monokai";

import locale    from 'react-json-editor-ajrm/locale/en';
import {
    Button,
    TextField,
    Grid,
    Paper
} from "@material-ui/core";

function ApiView(props) {
    const [script, setScript] = useState(""); // The script to run
    const [output, setOutput] = useState({}); // The output from the script

    function send_clicked(e) {
        e.preventDefault();

        Materia.req({ operation: "run", script: script }, (r) => {
            result = JSON.parse(r);
            if(result.result) {
                setOutput(r.result);
            } else {
                setOutput(r.error);
            }
        });
    }

    function handleScriptChange(newValue) {
        setScript(newValue); // newValue is the updated content of the editor
    }

    return (
        <Grid container direction="column" justify="space-around" alignItems="center" spacing={3}>
            <Grid item xs={12}>
                {/* Python Script Editor */}
                <Paper elevation={3} style={{ height: '40vh', width: '100%' }}>
                    <AceEditor
                        mode="python"
                        theme="monokai"
                        onChange={handleScriptChange}
                        name="scriptEditor"
                        editorProps={{ $blockScrolling: true }}
                        value={script}
                        height="100%"
                        width="100%"
                    />
                </Paper>
            </Grid>
            <Grid item>
                <Button variant="contained" color="primary" size="small" onClick={send_clicked}>Run Script</Button>
            </Grid>
            <Grid item xs={12}>
                {/* Script Output */}
                <Paper elevation={3} style={{ height: '40vh', width: '100%' }}>
                    <AceEditor
                        mode="json"
                        theme="monokai"
                        name="outputViewer"
                        editorProps={{ $blockScrolling: true }}
                        value={JSON.stringify(output, null, 2)}
                        height="100%"
                        width="100%"
                        readOnly={true}
                    />
                </Paper>
            </Grid>
        </Grid>
    );
}

export default ApiView;
