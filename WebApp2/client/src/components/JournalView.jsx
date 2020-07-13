import tinymce from 'tinymce/tinymce';
import 'tinymce/icons/default';
import 'tinymce/themes/silver';
import 'tinymce/plugins/searchreplace';
import 'tinymce/plugins/table';
import 'tinymce/plugins/lists';

/*tinymce.init({
    mode : "textareas",
    skin: "oxide-dark",
    selector: 'textarea',
    content_style: 'body { background-color: #242424; color: #dfe0e4; }'
  });*/

import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'
import TreeView from '@material-ui/lab/TreeView';
import TreeItem from '@material-ui/lab/TreeItem';

import DescriptionIcon from '@material-ui/icons/Description';
import FolderIcon from '@material-ui/icons/Folder';
import FolderOpenIcon from '@material-ui/icons/FolderOpen';

import { Editor } from '@tinymce/tinymce-react';

import {
    Grid
} from "@material-ui/core";

function sortIndex(src)
{
    //1 - x is greated
    return src.sort((x, y) => { 
        if(x.isPage === y.isPage)
        {
            if(x.title > y.title)
            {
                return 1;
            }
            else if(x.title < y.title)
            {
                return -1;
            }

            return 0;
        }
        else
        {
            return x.isPage === 'true' ? 1 : -1;
        }
    });
}

function JournalView(props) 
{
    const [index, setIndex] = useState(null);
    const [content, setContent] = useState("");

    if(index == null)
    {
        const req = {
            operation: "query",
            filter: 'IS(journal_header)'
        };

        MateriaRequest.req(JSON.stringify(req), (r) => {
            setIndex(sortIndex(JSON.parse(r).object_list));
        });
    }

    function fetchChildren(id)
    {
        var items = index.filter(x => {return x.parentFolderId === id;});

        if(items.length == 0)
        {
            return <div/>
        }

        return items.map((obj) => <TreeItem key={obj.id} 
            nodeId={obj.id} 
            label={obj.title} 
            endIcon={obj.isPage === 'true' ? <DescriptionIcon/> : <FolderIcon/>}
            collapseIcon={<FolderOpenIcon/>}
            expandIcon={<FolderIcon/>}>
                {obj.isPage === 'false' && fetchChildren(obj.id)}
        </TreeItem>);
    }

    function onNodeSelect(e, v)
    {
        var items = index.filter(x => {return x.id === v;});
        if(items[0].isPage === 'true')
        {
            const req = {
                operation: "query",
                filter: 'IS(journal_content) AND .headerId = "' + v + '"'
            };
    
            MateriaRequest.req(JSON.stringify(req), (r) => {
                setContent(JSON.parse(r).object_list[0].content);
            });
        }
    }

    return (<div> {index &&
        <Grid container direction="row" justify="space-around" alignItems="flex-start">
            <TreeView style={{width: '20vw'}} onNodeSelect={onNodeSelect}>
                {fetchChildren("")}
            </TreeView>
            <div style={{width: '75vw'}}>
                <Editor
                    value={content}
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
                    //onEditorChange={this.handleEditorChange}
                />
            </div>
        </Grid>
        }</div>
    );
}

export default JournalView;