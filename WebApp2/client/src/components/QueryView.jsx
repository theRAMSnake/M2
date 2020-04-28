import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'
import ObjectView from './ObjectView.jsx'

import {
    Box,
    CircularProgress,
    Typography
} from "@material-ui/core";

function createRequest(query)
{
    const req = {
        operation: "query",
        ids: [query]
    };

    return JSON.stringify(req);
}

function QueryView(props) 
{
    const query = props.query;
    const [responce, setResponce] = useState("");

    MateriaRequest.req(createRequest(query), (r) => {
        setResponce(r);
    });

    function getContentFromResponce()
    {
        var obj = JSON.parse(responce);
        if(obj.object_list.length == 0)
        {
            return (<Typography variant="h6">Nothing was found</Typography>);
        }
        else if(obj.object_list.length == 1)
        {
            return <ObjectView value={obj.object_list[0]}/>;
        }
        else
        {
            return (<Typography variant="h6">Cannot display results</Typography>);
        }
    }

    return (<Box style={{position: 'absolute', left: '50%', top: '50%', transform: 'translate(-50%, -50%)'}}>
                {responce === '' ? <CircularProgress /> : getContentFromResponce()}
            </Box>
    );
}

export default QueryView;