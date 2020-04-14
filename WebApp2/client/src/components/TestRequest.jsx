import React from 'react';
import { Card } from '@material-ui/core';
import { Button } from '@material-ui/core';
import MateriaRequest from '../modules/materia_request';

function TestRequest(props) {
    return (
        <Card className="container">
            <h2 className="card-heading">111</h2>
            <Button variant="contained" color="primary" type="button" onClick={() => { MateriaRequest.req(); }}>Request</Button>
        </Card>
    );
}

export default TestRequest;