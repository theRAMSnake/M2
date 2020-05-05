import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'
import JSONInput from 'react-json-editor-ajrm';
import locale    from 'react-json-editor-ajrm/locale/en';
import {
    Button,
    TextField,
    Grid
} from "@material-ui/core";

function ApiView(props) {

    const [lastRequest, setLastRequest] = useState("");
    const [lastResponce, setLastResponce] = useState({});

    function send_clicked(e) {
        e.preventDefault();

        MateriaRequest.req(lastRequest, (r) => {
            setLastResponce(JSON.parse(r));
        });
    }

    function reqChanged(e) {
        console.log(e);
        setLastRequest(e.json);
    }

    return (
            <Grid container direction="column" justify="space-around" alignItems="center">
                <JSONInput locale = { locale } height = '40vh' width='100%' onChange={reqChanged}/>
                <Button variant="contained" color="primary" size="small" onClick={send_clicked}>Send</Button>
                <JSONInput locale = { locale } height = '40vh' width='100%' viewOnly={true} placeholder={lastResponce}/>
            </Grid>
    );
}

export default ApiView;