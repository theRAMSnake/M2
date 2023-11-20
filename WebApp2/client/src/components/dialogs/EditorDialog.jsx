
import React, { useState } from 'react';
import {
    Dialog
} from '@material-ui/core';
import Paper from '@mui/material/Paper';
import AceEditor from 'react-ace';
import SaveIcon from '@material-ui/icons/Save';
import Fab from '@mui/material/Fab';
import 'ace-builds/src-noconflict/mode-json';
import 'ace-builds/src-noconflict/theme-monokai';

export default function EditorDialog(props)
{
    const [text, setText] = useState(props.text);
    const [changed, setChanged] = useState(false);

    return <Dialog open={true}
      onClose={props.onClose}
      maxWidth="md"
      fullWidth
      PaperProps={{
        style: {
          width: '50%',
          height: '70%',
          display: 'flex',
          flexDirection: 'column',
          justifyContent: 'center',
          alignItems: 'center',
        },
      }}
    >
      <Paper elevation={3} style={{ height: '100%', width: '100%' }}>
        <AceEditor
          mode={props.mode}
          theme="monokai" // Choose your preferred theme
          onChange={(newValue) => {setText(newValue);setChanged(true);}}
          name="json-editor"
          editorProps={{ $blockScrolling: true }}
          value={text}
          height="100%"
          showPrintMargin={false}
          width="100%"
          setOptions={{
            tabSize: 2, // Adjust the tab size for indentation
            useSoftTabs: true, // Use soft tabs (spaces) for indentation
            wrap: true, // Enable line wrapping
          }}
        />
        {changed && <Fab sx={{position: 'absolute', bottom: 16, right: 16}} color="primary" onClick={() => props.onSave(text)}>
          <SaveIcon/>
        </Fab>}
      </Paper>
    </Dialog>
}
