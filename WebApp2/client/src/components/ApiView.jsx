import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'

import {
    Button,
    TextField,
    Grid
} from "@material-ui/core";

function ApiView(props) {

    const [lastRequest, setLastRequest] = useState("");
    const [lastResponce, setLastResponce] = useState("");

    function send_clicked(e) {
        e.preventDefault();

        MateriaRequest.req(lastRequest, (r) => {
            setLastResponce(r);
        });
    }

    function reqChanged(e) {
        e.preventDefault();
        setLastRequest(e.target.value)
    }

    return (
            <Grid container direction="column" justify="space-around" alignItems="center">
                <TextField id="id_input" label="Input" variant="outlined" multiline="true" rows="14" fullWidth="true" margin="normal" inputProps={{onChange:reqChanged}} />
                <Button variant="contained" color="primary" size="small" onClick={send_clicked}>Send</Button>
                <TextField id="id_output" label="Output" variant="outlined" multiline="true" rows="14" fullWidth="true" margin="normal" inputProps={{readOnly:true}} value={lastResponce}/>
            </Grid>
    );
}

export default ApiView;