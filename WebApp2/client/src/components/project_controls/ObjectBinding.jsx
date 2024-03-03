import React, {useState, useRef, useEffect} from 'react';
import RichEditor from '../RichEditor.jsx'
import Materia from '../../modules/materia_request'

function replaceTemplateString(content, state) {
  return content.replace(/\${(.*?)}/g, (match, path) => {
    const keys = path.split('.');

    const fetchValue = (obj, keyPath) => {
      return keyPath.reduce((current, key, index) => {
        if (Array.isArray(current)) {
          return current.map(item => fetchValue(item, keyPath.slice(index)));
        } else if (current && typeof current === 'object' && key in current) {
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
      const listItems = result.map(item => `<li>${item}</li>`).join('');
      return `<ul>${listItems}</ul>`;
    } else {
      return result;
    }
  });
}

const ObjectBinding = ({ control}) => {
    const [object, setObject] = useState(null);
    useEffect(() => {
      const req = {
          operation: "query",
          ids: [control.binding]
      };

      Materia.exec(req, (r) =>
      {
          setObject(r.object_list[0]);
      });
    }, [control.binding]);
    return (
        <div dangerouslySetInnerHTML={{__html: replaceTemplateString(control.value, object)}}/>
      );
};

export class ObjectBindingEditor extends React.Component {
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

    handleBindingChange = (e) => {
        this.setState(prevState => ({
            myObject : {...prevState.myObject, binding: e.target.value}
        }));
    }

    render() {
        const { control } = this.props;

        return (
            <div>
                <TextField inputProps={{onChange: this.handleBindingChange}} value={myObject.binding} fullWidth label="Binding" />
                <RichEditor
                    initialValue={control.value}
                    getRefCb={(editor) => this.editorRef = editor}
                />
            </div>
        );
    }
}

export default ObjectBinding;
