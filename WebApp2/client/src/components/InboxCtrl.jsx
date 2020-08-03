import Materia from '../modules/materia_request'
import React from 'react';
import ListObjectView from './ListObjectView.jsx'

import MailIcon  from '@material-ui/icons/Mail';

import {
    IconButton,
    Badge
} from "@material-ui/core";

export default function InboxCtrl(props) 
{
    const [inbox, setInbox] = React.useState({objects: []});
    const [init, setInit] = React.useState(false);
    const [inDialog, setInDialog] = React.useState(false);

    if(!init)
    {
        const inboxReq = {
            operation: "query",
            ids: ['inbox']
        };
        
        Materia.exec(inboxReq, (r) => {
            setInbox(r.object_list[0]);
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
                <IconButton aria-label="inbox" color="inherit"onClick={prepareDialog}>
                    <Badge badgeContent={inbox.objects.length} color="secondary">
                        <MailIcon />
                    </Badge>
                </IconButton>
                <ListObjectView open={inDialog} object={inbox} onChange={onObjectChanged} onClose={onDialogClose}/>
            </div>;
}