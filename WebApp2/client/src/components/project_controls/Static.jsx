import React, {useState, useRef} from 'react';
import RichEditor from '../RichEditor.jsx'

function replaceTemplateString(content, state) {
  return content.replace(/\${(.*?)}/g, (match, path) => {
    // Split the path into its components
    const keys = path.split('.');

    // Reduce the keys to get the final value
    const result = keys.reduce((currentObject, key) => {
      // Check if the currentObject is valid and has the key
      return currentObject !== null && currentObject !== undefined ? currentObject[key] : '';
    }, state);

    // Check if the result is an array and convert it to an HTML list if it is
    if (Array.isArray(result)) {
      const listItems = result.map(item => `<li>${item[keys[keys.length - 1]]}</li>`).join('');
      return `<ul>${listItems}</ul>`;
    } else {
      return result;
    }
  });
}

const Static = ({ control, state }) => {
    return (
        <div dangerouslySetInnerHTML={{__html: replaceTemplateString(control.value, state)}}/>
      );
};

export class StaticEditor extends React.Component {
    constructor(props) {
        super(props);
        this.editorRef = null;
    }

    getContent() {
        const { control } = this.props;
        if (this.editorRef) {
            return {...control, value: this.editorRef.getContent()};
        }
    }

    render() {
        const { control } = this.props;

        return (
            <RichEditor
                initialValue={control.value}
                getRefCb={(editor) => this.editorRef = editor}
            />
        );
    }
}

export default Static;
