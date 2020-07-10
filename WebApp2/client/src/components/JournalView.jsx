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

    return (
        <div> {index &&
        <Grid container direction="row" justify="space-around" alignItems="flex-start">
            <TreeView style={{width: '20vw'}}>
                {fetchChildren("")}
            </TreeView>
            <div style={{width: '75vw'}}>
            <Editor
                initialValue="<p>This is the initial content of the editor</p>"
                init={{
                    height: '80vh',
                    menubar: "edit format table",
                    statusbar: false,
                    plugins: [
                        'colorpicker, textcolor, searchreplace, table, lists'
                    ],
                    toolbar:
                        'bold italic | link | forecolor backcolor | fontsizeselect | numlist bullist'
                    }}
                //onEditorChange={this.handleEditorChange}
                //"menubar", "edit format table"
            />
            </div>
        </Grid>
        }</div>
    );
}

export default JournalView;