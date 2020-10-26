const express = require('express');
const bodyParser = require('body-parser');
const passport = require('passport');
const config = require('./config');
const app = express();
var fs = require('fs');

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

if(config.UseHttps)
{
    const https = require('https');
    var privateKey  = fs.readFileSync('/etc/letsencrypt/live/ramsnake.net-0002/privkey.pem', 'utf8');
    var certificate = fs.readFileSync('/etc/letsencrypt/live/ramsnake.net-0002/fullchain.pem', 'utf8');
    var credentials = {key: privateKey, cert: certificate};
    var httpsServer = https.createServer(credentials, app);
    httpsServer.listen(5758);
}
else
{
    app.listen(5758);
}