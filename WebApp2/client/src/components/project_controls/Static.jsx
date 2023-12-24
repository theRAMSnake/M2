import React, {useState, useRef} from 'react';
import RichEditor from '../RichEditor.jsx'

function replaceTemplateString(content, state) {
  console.log(content)
  return content.replace(/\${(.*?)}/g, (match, path) => {
    const keys = path.split('.');

    const fetchValue = (obj, keyPath) => {
      return keyPath.reduce((current, key) => {
        if (current && typeof current === 'object' && key in current) {
          return current[key];
        }
        return undefined;
      }, obj);
    };

    const result = fetchValue(state, keys);

    if (result === undefined) {
      return '';
    }

    if (Array.isArray(result)) {
      const listItems = result.map(item => {
        if (typeof item === 'object') {
          return `<li>${fetchValue(item, keys.slice(1))}</li>`;
        }
        return `<li>${item}</li>`;
      }).join('');
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
