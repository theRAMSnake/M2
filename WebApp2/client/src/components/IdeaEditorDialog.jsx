import React, { useState } from 'react';

import tinymce from 'tinymce/tinymce';
import 'tinymce/icons/default';
import 'tinymce/themes/silver';
import 'tinymce/plugins/searchreplace';
import 'tinymce/plugins/table';
import 'tinymce/plugins/lists';

tinymce.init({
    mode : "textareas",
    skin: "oxide-dark",
    selector: 'textarea',
    content_style: 'body { background-color: #242424; color: #dfe0e4; }'
  });

import { Editor } from '@tinymce/tinymce-react';

import {
    Dialog,
    DialogTitle,
    DialogContent,
    Button,
    DialogActions,
    TextField
} from '@material-ui/core'

function initHashtags(htgs)
{
    if(htgs)
    {
        return htgs.join(" ");
    } 
    else
    {
        return "";
    }
}

function parseHtgs(s)
{
    return s.split(' ');
}

function IdeaEditorDialog(props)
{
    const [title, setTitle] = useState(props.object.title);
    const [hashtags, setHashtags] = useState(initHashtags(props.object.hashtags));
    const [content, setContent] = useState(props.object.content);

    function onTitleChanged(e)
    {
        setTitle(e.target.value);
    }

    function onHashtagsChanged(e)
    {
        setHashtags(e.target.value);
    }

    return (
        <Dialog fullWidth open={true} onClose={props.onCancel} aria-labelledby="dialog-title">
            <DialogTitle id="dialog-title">Idea editor</DialogTitle>
            <DialogContent>
                <TextField id="id_title" label="Title" fullWidth value={title} inputProps={{onChange: onTitleChanged}}/>
                <Editor
                    value={content}
                    init={{
                        selector: 'textarea',
                        height: '60vh',
                        menubar: "edit format table",
                        content_style: 'body { background-color: #242424; color: #dfe0e4; }',
                        statusbar: false,
                        skin: "oxide-dark",
                        plugins: [
                            'searchreplace, table, lists'
                        ],
                        toolbar:
                            'bold italic | link | forecolor backcolor | fontsizeselect | numlist bullist'
                        }}
                    onEditorChange={(e) => setContent(e)}
                />
                <TextField id="id_hashtags" label="Hashtags" fullWidth value={hashtags} inputProps={{onChange: onHashtagsChanged}}/>
            </DialogContent>
            <DialogActions>
                <Button onClick={props.onCancel} variant="contained" color="primary">
                    Cancel
                </Button>
                <Button onClick={() => props.onOk(title, parseHtgs(hashtags), content)} variant="contained" color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>
        </Dialog>
    );
}

export default IdeaEditorDialog;