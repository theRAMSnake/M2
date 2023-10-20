import React, { useState } from 'react';
import Materia from '../modules/materia_request'
import AceEditor from 'react-ace';
import "ace-builds/src-noconflict/mode-python";
import "ace-builds/src-noconflict/theme-monokai";
import 'ace-builds/src-noconflict/mode-text';

import locale    from 'react-json-editor-ajrm/locale/en';
import {
    Button,
    TextField,
    Grid,
    Paper
} from "@material-ui/core";

function ApiView(props) {
    const [script, setScript] = useState(""); // The script to run
    const [output, setOutput] = useState(""); // The output from the script

    function send_clicked(e) {
        e.preventDefault();

        Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
           let result = JSON.parse(r);
           if(result.result) {
              setOutput(result.result);
           } else {
              setOutput(result.error);
           }
        }, (err) => {
           setOutput(err);
        });
    }

    function handleScriptChange(newValue) {
        setScript(newValue); // newValue is the updated content of the editor
    }

    return (
        <Grid container direction="column" justify="space-around" alignItems="center">
                {/* Python Script Editor */}
                <Paper elevation={3} style={{ height: '50vh', width: '100%' }}>
                    <AceEditor
                        mode="python"
                        theme="monokai"
                        onChange={handleScriptChange}
                        name="scriptEditor"
                        showPrintMargin={false}
                        editorProps={{ $blockScrolling: true }}
                        value={script}
                        height="100%"
                        width="100%"
                    />
                </Paper>
                <Button variant="contained" color="primary" size="small" onClick={send_clicked}>Run Script</Button>
                {/* Script Output */}
                <Paper elevation={3} style={{ height: '30vh', width: '100%' }}>
                    <AceEditor
                        mode="text"
                        theme="monokai"
                        name="outputViewer"
                        editorProps={{ $blockScrolling: true }}
                        showPrintMargin={false}
                        value={output}
                        height="100%"
                        width="100%"
                        readOnly={true}
                    />
                </Paper>
        </Grid>
    );
}

export default ApiView;
