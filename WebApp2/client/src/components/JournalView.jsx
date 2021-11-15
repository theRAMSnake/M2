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

import React, { useState } from 'react';
import Materia from '../modules/materia_request'
import TreeView from '@material-ui/lab/TreeView';
import TreeItem from '@material-ui/lab/TreeItem';

import DescriptionIcon from '@material-ui/icons/Description';
import FolderIcon from '@material-ui/icons/Folder';
import FolderOpenIcon from '@material-ui/icons/FolderOpen';

import { Editor } from '@tinymce/tinymce-react';

import {
    Grid,
    CircularProgress,
    Backdrop,
    IconButton
} from "@material-ui/core";

import SaveIcon from '@material-ui/icons/Save';
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import EditIcon from '@material-ui/icons/Edit';
import ClearAllIcon from '@material-ui/icons/ClearAll';
import AddJournalItemDialog from './AddJournalItemDialog.jsx'
import TextQueryDialog from './dialogs/TextQueryDialog.jsx'

function sortHeaders(items, conns)
{
    //1 - x is greated
    return items.sort((x, y) => { 
        const xIsPage = conns.filter(i => i.A === x.id && i.type === "Extension").length != 0;
        const yIsPage = conns.filter(i => i.A === y.id && i.type === "Extension").length != 0;
        if(xIsPage == yIsPage)
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
            return xIsPage ? 1 : -1;
        }
    });
}

function JournalView(props) 
{
    const [updating, setUpdating] = useState(true);
    const [headers, setHeaders] = useState(null);
    const [conns, setConns] = useState(null);
    const [curPage, setCurPage] = useState(null);
    const [content, setContent] = useState("");
    const [changed, setChanged] = useState(false);
    const [inSaveDialog, setInSaveDialog] = useState(false);
    const [inDeleteDialog, setInDeleteDialog] = useState(false);
    const [inClearDialog, setInClearDialog] = useState(false);
    const [inEditDialog, setInEditDialog] = useState(false);
    const [inAddDialog, setInAddDialog] = useState(false);
    const [selectedId, setSelectedId] = useState("");
    const [selectedItemIsPage, setSelectedItemIsPage] = useState(true);

    function updateHeaders()
    {
        const req = {
            operation: "query",
            filter: 'IS(journal_header)'
        };

        Materia.exec(req, (r) => {
            setConns(r.connection_list);
            setHeaders(sortHeaders(r.object_list, r.connection_list));
            setUpdating(false);
        });
    }

    if(headers == null)
    {
        updateHeaders();
    }

    function fetchChildren(id)
    {
        var items = []
        if(id === "")
        {
            items = headers.filter(x => {return conns.findIndex(y => y.B === x.id && y.type === "Hierarchy") == -1;});
        }
        else
        {
            var childrenIds = conns.filter(x => x.A === id && x.type === "Hierarchy").map(x => x.B);
            items = headers.filter(x => {return childrenIds.findIndex(y => y === x.id) != -1;})
        }

        if(items.length == 0)
        {
            return <div/>
        }

        return items.map((obj) => {
            const isPage = conns.filter(i => i.A === obj.id && i.type === "Extension").length != 0;

            return <TreeItem key={obj.id} 
            nodeId={obj.id} 
            label={obj.title} 
            endIcon={isPage ? <DescriptionIcon/> : <FolderIcon/>}
            collapseIcon={<FolderOpenIcon/>}
            expandIcon={<FolderIcon/>}>
                {!isPage && fetchChildren(obj.id)}
            </TreeItem>
        });
    }

    function afterSelect(id)
    {
        setSelectedId(id);
        const isPage = conns.filter(i => i.A === id && i.type === "Extension").length != 0;
        if(isPage)
        {
            setSelectedItemIsPage(true);
            setUpdating(true);
            
            const req = {
                operation: "query",
                filter: 'IS(journal_content) AND Extends(' + id + ')'
            };
    
            Materia.exec(req, (r) => {
                var o = r.object_list[0];
                setCurPage(o);
                setContent(o.content);
                setChanged(false);
                setUpdating(false);
            });
        }
        else
        {
            setSelectedItemIsPage(false);
        }
    }

    function onNodeSelect(e, v)
    {
        if(changed)
        {
            setInSaveDialog(true);
            setSelectedId(v);
            return;
        }

        afterSelect(v);
    }

    function handleEditorChange(content, editor)
    {
        curPage.content = content;
        setChanged(true);
    }

    function onSaveClicked()
    {
        Materia.postEdit(curPage.id, JSON.stringify(curPage));
        setChanged(false);
    }

    function onSaveDialogCancel()
    {
        setInSaveDialog(false);
        setChanged(false);
        afterSelect(selectedId);
    }

    function onSaveDialogOk()
    {
        setInSaveDialog(false);
        onSaveClicked();
        afterSelect(selectedId);
    }

    function onAddClicked()
    {
        setInAddDialog(true);
    }

    function onClearClicked()
    {
        setInClearDialog(true);
    }

    function onDeleteClicked()
    {
        setInDeleteDialog(true);
    }

    function onEditClicked()
    {
        setInEditDialog(true);
    }

    function onClearDialogCancel()
    {
        setInClearDialog(false);
    }

    function onClearDialogOk()
    {
        return ;
        setInClearDialog(false);
        setUpdating(true);

        var items = headers.filter(x => {return x.parentFolderId === selectedId;});
        items.forEach(element => {
            Materia.postDelete(element.id);
        });

        updateHeaders();
    }

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
    }

    function onDeleteDialogOk()
    {
        setInDeleteDialog(false);
        setUpdating(true);

        Materia.postDelete(selectedId);

        updateHeaders();
    }

    function onAddDialogCancel()
    {
        setInAddDialog(false);
    }

    function onAddDialogOk(newtitle, isPage)
    {
        return ;
        setInAddDialog(false);
        setUpdating(true);

        {
            var obj = {
                title: newtitle,
                isPage: false,
                parentFolderId: selectedItemIsPage ? "" : selectedId
            }
    
            var req = {
                operation: "create",
                typename: "journal_header",
                params: obj
            }
    
            Materia.exec(req, (x) => {
                if(isPage)
                {
                    var sobj = {
                        headerId: x.result_id
                    }
            
                    var sreq = {
                        operation: "create",
                        typename: "journal_content",
                        params: sobj
                    }
            
                    Materia.post(sreq);
                }

                updateHeaders();
            });
        }
    }

    function getCurTitle()
    {
        var items = headers.filter(x => {return x.id === selectedId;});
        return items[0].title;
    }

    function handleRenameFinished(text)
    {
        setInEditDialog(false);
        setUpdating(true);

        var items = headers.filter(x => {return x.id === selectedId;});
        var obj = items[0];
        obj.title = text;

        Materia.postEdit(obj.id, JSON.stringify(obj));
        updateHeaders();
    }

    function handleRenameCanceled()
    {
        setInEditDialog(false);
    }

    return (<div>
        <Backdrop open={updating}><CircularProgress color="inherit"/></Backdrop>
        <AddJournalItemDialog open={inAddDialog} onCancel={onAddDialogCancel} onOk={onAddDialogOk}/>
        <ConfirmationDialog open={inSaveDialog} question="save unsaved changes" caption="confirm save unsaved changes" onNo={onSaveDialogCancel} onYes={onSaveDialogOk} />
        <ConfirmationDialog open={inClearDialog} question="clear" caption="confirm clear" onNo={onClearDialogCancel} onYes={onClearDialogOk} />
        <ConfirmationDialog open={inDeleteDialog} question="delete" caption="confirm delete" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
        {inEditDialog && <TextQueryDialog text={getCurTitle()} onFinished={handleRenameFinished} onCanceled={handleRenameCanceled}/>}
        {headers &&
        <Grid container direction="row" justify="space-around" alignItems="flex-start">
            <div>
            <IconButton edge="start" onClick={onAddClicked}>
                <AddCircleOutlineIcon/>
            </IconButton>
            <IconButton edge="start" onClick={onDeleteClicked}>
                <DeleteForeverIcon/>
            </IconButton>
            <IconButton edge="start" onClick={onEditClicked}>
                <EditIcon/>
            </IconButton>
            <IconButton edge="start" onClick={onClearClicked} disabled={selectedItemIsPage}>
                <ClearAllIcon/>
            </IconButton>
            <TreeView style={{width: '20vw'}} onNodeSelect={onNodeSelect}>
                {fetchChildren("")}
            </TreeView>
            </div>
            <div style={{width: '75vw'}}>
                <IconButton edge="start" onClick={onSaveClicked} disabled={!changed}>
                    <SaveIcon/>
                </IconButton>
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
                    onEditorChange={handleEditorChange}
                />
            </div>
        </Grid>
        }</div>
    );
}

export default JournalView;
