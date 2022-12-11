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
import MateriaConnections from '../modules/connections'
import TreeView from '@material-ui/lab/TreeView';
import TreeItem from '@material-ui/lab/TreeItem';

import DescriptionIcon from '@material-ui/icons/Description';
import FolderIcon from '@material-ui/icons/Folder';
import FolderOpenIcon from '@material-ui/icons/FolderOpen';
import TrendingFlatIcon from '@material-ui/icons/TrendingFlat';
import DoneIcon from '@material-ui/icons/Done';

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
        const xIsPage = conns.Has(x.id, "ExtendedBy", "*");
        const yIsPage = conns.Has(y.id, "ExtendedBy", "*");
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
    const [inMoveMode, setInMoveMode] = useState(false);
    const [inAddDialog, setInAddDialog] = useState(false);
    const [selectedId, setSelectedId] = useState("");
    const [moveSubjectId, setMoveSubjectId] = useState("");
    const [selectedItemIsPage, setSelectedItemIsPage] = useState(true);

    function updateHeaders()
    {
        const req = {
            operation: "query",
            filter: 'IS(journal_header)'
        };

        Materia.exec(req, (r) => {
            var cons = new MateriaConnections(r.connection_list);
            setConns(cons);
            setHeaders(sortHeaders(r.object_list, cons));
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
            items = headers.filter(x => !conns.Has("*", "ParentOf", x.id));
        }
        else
        {
            items = headers.filter(x => conns.Has(id, "ParentOf", x.id));
        }

        if(items.length == 0)
        {
            return <div/>
        }

        return items.map((obj) => {
            const isPage = conns.Has(obj.id, "ExtendedBy", "*");

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
        const isPage = conns.Has(id, "ExtendedBy", "*");
        if(isPage)
        {
            setSelectedItemIsPage(true);
            setUpdating(true);
            
            const req = {
                operation: "query",
                filter: 'IS(journal_content) AND Extends("' + id + '")'
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
        setInClearDialog(false);
        setUpdating(true);

        var items = headers.filter(x => conns.Has(selectedId, "ParentOf", x.id));

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
        setInAddDialog(false);
        setUpdating(true);

        {
            var obj = {
                title: newtitle
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
                        content: ""
                    }

                    var sreq = {
                        operation: "create",
                        typename: "journal_content",
                        params: sobj
                    }

                    Materia.exec(sreq, (y) => {

                        var conReq = {
                            operation: "create",
                            typename: "connection",
                            params: {
                                A: x.result_id,
                                B: y.result_id,
                                type: "Extension"
                            }
                        }

                        Materia.exec(conReq, (x) => {updateHeaders();});

                    });
                }

                var parentId = null;
                if(!selectedItemIsPage)
                {
                    parentId = selectedId;
                }
                else
                {
                    //Use parent of selectedItem as a parent
                    if(conns.Has("*", "ParentOf", selectedId))
                    {
                        parentId = conns.AllOf("*", "ParentOf", selectedId)[0].A;
                    }
                }

                if(parentId)
                {
                    var conReq2 = {
                        operation: "create",
                        typename: "connection",
                        params: {
                            A: parentId,
                            B: x.result_id,
                            type: "Hierarchy"
                        }
                    }
                    Materia.exec(conReq2, (x) => {updateHeaders();});
                }
                else
                {
                    updateHeaders();
                }
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

    function onMoveClicked()
    {
        setInMoveMode(true);
        setMoveSubjectId(selectedId);
    }

    function onMoveDoneClicked()
    {
        setInMoveMode(false);
        if(selectedId !== moveSubjectId)
        {
            var newParentId = null;
            if(!selectedItemIsPage)
            {
                newParentId = selectedId;
            }
            else
            {
                //Use parent of selectedItem as a parent
                if(conns.Has("*", "ParentOf", selectedId))
                {
                    newParentId = conns.AllOf("*", "ParentOf", selectedId)[0].A;
                }
            }

            var oldParentId = null;
            if(conns.Has("*", "ParentOf", moveSubjectId))
            {
                oldParentId = conns.AllOf("*", "ParentOf", moveSubjectId)[0].A;
            }

            if(oldParentId !== newParentId)
            {
                if(oldParentId)
                {
                    var oldConnId = conns.AllOf("*", "ParentOf", moveSubjectId)[0].id;
                    Materia.sendDelete(oldConnId, x => {updateHeaders();});
                }

                if(newParentId)
                {
                    Materia.createConnection(newParentId, moveSubjectId, "Hierarchy", result_id => {updateHeaders();});
                }
            }
        }
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
            {
                inMoveMode &&
                    <IconButton edge="start" onClick={onMoveDoneClicked}>
                        <DoneIcon/>
                    </IconButton>
            }
            {
                !inMoveMode &&
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
                    <IconButton edge="start" onClick={onMoveClicked} disabled={!selectedId}>
                        <TrendingFlatIcon/>
                    </IconButton>
                    </div>
            }
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
