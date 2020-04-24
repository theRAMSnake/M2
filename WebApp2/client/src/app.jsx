import React from 'react';
import ReactDom from 'react-dom';
import LoginPage from './components/LoginPage.jsx';
import MainPage from './components/MainPage.jsx';
import Auth from './modules/Auth';
import { ThemeProvider } from "@material-ui/styles";

import {
    CssBaseline,
    createMuiTheme
} from "@material-ui/core";
  
const theme = createMuiTheme({
    palette: {
        type: "dark",
        primary: { main: '#ff4400' }
    }
});

class App extends React.Component {
    constructor(props) {
        super(props);
        this.state = {auth: Auth.isUserAuthenticated()};

        this.onAuthChange = this.onAuthChange.bind(this);

        Auth.setCallback(this.onAuthChange);
    }

    onAuthChange(){
        this.setState({
            auth: Auth.isUserAuthenticated()
        });
    }

    render() {
        return (
            <ThemeProvider theme={theme}>
                <CssBaseline />
                {this.state.auth ? <MainPage/> : <LoginPage/>}
            </ThemeProvider>
        );
    }
}

ReactDom.render(
    <App/>, document.getElementById('react-app'));