import React, {useState, useRef} from 'react';
import RichEditor from '../RichEditor.jsx'

const Static = ({ control }) => {
    return (
        <div dangerouslySetInnerHTML={__html: control.value}/>
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
