import React from 'react';
import { Card } from '@material-ui/core';
import { Button } from '@material-ui/core';
import {TextField} from '@material-ui/core';
import Auth from '../modules/Auth';

function LoginForm(props) {
    const inputProps = {
        onChange: props.onChange,
      };

    return (
        <Card className="container">
            <form action="/" onSubmit={props.onSubmit}>
            <h2 className="card-heading">Welcome back, Snake!</h2>
        
                <div className="field-line">
                <TextField
                    id="standard-password-input"
                    label="Password"
                    type="password"
                    inputProps={inputProps}
                />
                </div>
        
                {!props.auth ?
                <div className="button-line">
                    <Button variant="contained" color="primary" type="submit">Log In</Button>
                </div> :
                <div className="button-line">
                    <Button variant="contained" color="primary" type="submit">Log Out</Button>
                </div>}
            </form>
        </Card>
    );
}

export default LoginForm;