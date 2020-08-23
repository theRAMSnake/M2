const express = require('express');
const bodyParser = require('body-parser');
const passport = require('passport');
const config = require('./config');
const app = express();
var https = require('https');
var privateKey  = fs.readFileSync('???.key', 'utf8');
var certificate = fs.readFileSync('???.crt', 'utf8');

var credentials = {key: privateKey, cert: certificate};

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

var httpsServer = https.createServer(credentials, app);

httpsServer.listen(5758);