const express = require('express');
const bodyParser = require('body-parser');
const passport = require('passport');
const config = require('./config');
const app = express();

app.use(express.static('./server/static/'));
app.use(express.static('./client/dist/'));
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json())
app.use(passport.initialize());

const localLoginStrategy = require('./server/passport/local-login');
passport.use(localLoginStrategy);

const authCheckMiddleware = require('./server/middleware/auth-check');
app.use('/api', authCheckMiddleware);

const authRoutes = require('./server/routes/auth');
const apiRoutes = require('./server/routes/api');
app.use('/auth', authRoutes);
app.use('/api', apiRoutes);

app.listen(5758, () => {
    console.log('Server is running on http://localhost:5758 or http://127.0.0.1:5758');
  });