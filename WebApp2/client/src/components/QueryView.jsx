import React, { useState } from 'react';
import Materia from '../modules/materia_request'
import ObjectGrid from './ObjectGrid.jsx'

import {
    Box,
    CircularProgress,
    Typography
} from "@material-ui/core";

function createRequest(query)
{
    if(query.charAt(0) === '>')
    {
        const req = {
            operation: "query",
            filter: query.substr(1)
        };
    
        return req;
    }
    else
    {
        const req = {
            operation: "query",
            ids: [query]
        };
    
        return req;
    }
}

function QueryView(props) 
{
    const query = props.query;
    const [responce, setResponce] = useState("");

    Materia.exec(createRequest(query), (r) => {
        setResponce(r);
    });

    function getContentFromResponce()
    {
        var obj = responce;
        if(!obj || !obj.object_list || obj.object_list.length == 0)
        {
            return (
                <Box style={{position: 'absolute', left: '50%', top: '50%', transform: 'translate(-50%, -50%)'}}>
                    <Typography variant="h6">Nothing was found</Typography>
                </Box>
            );
        }

        return (<ObjectGrid content={obj.object_list} />);
    }

    return (responce === '' ? 
        <Box style={{position: 'absolute', left: '50%', top: '50%', transform: 'translate(-50%, -50%)'}}>
            <CircularProgress />
        </Box> 
        : 
        getContentFromResponce()
    );
}

export default QueryView;