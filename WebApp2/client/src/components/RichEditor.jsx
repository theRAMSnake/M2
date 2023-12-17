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
import React, {useState} from 'react';

const RichEditor = ({ initialValue, getRefCb }) => {
    return (
        <Editor
            initialValue={initialValue}
            onInit={(evt, editor) => getRefCb(editor)}
            init={{
                selector: 'textarea',
                height: '80vh',
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
        />
      );
};

export default RichEditor;
