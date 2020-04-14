import React from 'react';
import LoginForm from './LoginForm.jsx';
import Auth from '../modules/Auth';
import TestRequest from './TestRequest.jsx'

class LoginPage extends React.Component {
    constructor(props) {
        super(props);
    
        this.processForm = this.processForm.bind(this);
        this.onChange = this.onChange.bind(this);

        this.state = {password: '', auth: Auth.isUserAuthenticated()}
    }

    processForm(event) {
        // prevent default action. in this case, action is the form submission event
        event.preventDefault();

        if(Auth.isUserAuthenticated())
        {
            Auth.deauthenticateUser();
            this.setState({auth: Auth.isUserAuthenticated()});
        }
        else
        {
            const password = encodeURIComponent(this.state.password);
            const formData = `uname=snake&password=${password}`;
    
            const xhr = new XMLHttpRequest();
            xhr.open('post', '/auth/login');
            xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
            xhr.responseType = 'json';
            xhr.addEventListener('load', () => {
                if (xhr.status === 200) 
                {
                    Auth.authenticateUser(xhr.response.token);
                } 
                else 
                {
                    Auth.deauthenticateUser();
                }

                this.setState({auth: Auth.isUserAuthenticated()});
            });
            xhr.send(formData);
        }
    }

    onChange(event) {
        this.setState({password: event.target.value});
    }

    render() {
        return (
            <div>
                <LoginForm onSubmit={this.processForm} onChange={this.onChange} password='' auth={this.state.auth}/>
                <TestRequest/>
            </div>
          );
    }
  }

export default LoginPage;