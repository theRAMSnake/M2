const jwt = require('jsonwebtoken');
const LocalStrategy = require('passport-local').Strategy;
const config = require('../../config');
const pass = require("../pass");

module.exports = new LocalStrategy({
    usernameField: 'uname',
    passwordField: 'password',
    session: false,
    passReqToCallback: true
  }, (req, uname, password, done) => {

    if (password && password === pass) 
    {
        const payload = {
            sub: '0'
        };
    
        // create a token string
        const token = jwt.sign(payload, pass, { expiresIn: '1 day' });
        const user = {
            name: 'snake'
          };
        return done(null, user, token);
    }
    else
    {
        const error = new Error('Incorrect password');
        error.name = 'IncorrectCredentialsError';

        return done(error);
    }
});