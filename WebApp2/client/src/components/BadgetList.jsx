import Materia from '../modules/materia_request'
import React from 'react';
import ListObjectView from './ListObjectView.jsx'

import {
    IconButton,
    Badge
} from "@material-ui/core";

export default function BadgetList(props) 
{
    const [list, setList] = React.useState({objects: []});
    const [init, setInit] = React.useState(false);
    const [inDialog, setInDialog] = React.useState(false);

    if(!init)
    {
        const req = {
            operation: "query",
            ids: [props.id]
        };
        
        Materia.exec(req, (r) => {
            setList(r.object_list[0]);
        });

        setInit(true);
    }

    function prepareDialog()
    {
        setInDialog(true);
    }

    function onObjectChanged(newObj)
    {
        Materia.postEdit(newObj.id, JSON.stringify(newObj));
        setObject(JSON.parse(JSON.stringify(newObj)));
    }

    function onDialogClose()
    {
        setInDialog(false);
    }   

    return <div>
                <IconButton aria-label="icon" color="inherit" onClick={prepareDialog}>
                    <Badge badgeContent={list.objects.length} color="secondary">
                        <props.icon/>
                    </Badge>
                </IconButton>
                <ListObjectView open={inDialog} object={list} onChange={onObjectChanged} onClose={onDialogClose}/>
            </div>;
}