import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'
import TreeView from '@material-ui/lab/TreeView';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';
import ChevronRightIcon from '@material-ui/icons/ChevronRight';
import TreeItem from '@material-ui/lab/TreeItem';

function JournalView(props) 
{
    const [index, setIndex] = useState(null);

    if(index == null)
    {
        const req = {
            operation: "query",
            filter: ['IS(journal_header) AND .parentFolderId = ""']
        };

        MateriaRequest.req(JSON.stringify(req), (r) => {
            setIndex(JSON.parse(r).object_list[0]);
        });
    }

    return (
        <div> {index &&
        <TreeView>
            {index.map((obj) => <TreeItem nodeId={obj.id}>{obj.title}</TreeItem>)}
        </TreeView>}
        </div>
    );
}

export default JournalView;