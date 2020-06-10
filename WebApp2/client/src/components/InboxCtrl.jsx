import MateriaRequest from '../modules/materia_request'
import React from 'react';

import MailIcon  from '@material-ui/icons/Mail';

import {
    IconButton,
    Badge
} from "@material-ui/core";

export default function InboxCtrl(props) 
{
    const [inbox, setInbox] = React.useState({objects: []});
    const inboxReq = {
        operation: "query",
        ids: ['inbox']
    };
    
    MateriaRequest.req(JSON.stringify(inboxReq), (r) => {
        setInbox(JSON.parse(r).object_list[0]);
    });

    return  <IconButton aria-label="inbox" color="inherit">
                <Badge badgeContent={inbox.objects.length} color="secondary">
                    <MailIcon />
                </Badge>
            </IconButton>;
}