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

/*
import tinymce from 'tinymce/tinymce';
import 'tinymce/icons/default';
import 'tinymce/themes/silver';
import 'tinymce/plugins/colorpicker';
import 'tinymce/plugins/searchreplace';
import 'tinymce/plugins/table';
import 'tinymce/plugins/lists';
import 'tinymce/plugins/textcolor';
*/

/*var tinymce = require('tinymce');
require('tinymce/icons/default');
require('tinymce/themes/silver');
require('tinymce/plugins/colorpicker');
require('tinymce/plugins/searchreplace');
require('tinymce/plugins/table');
require('tinymce/plugins/lists');
require('tinymce/plugins/textcolor');

// Initialize the app
tinymce.init({
  selector: '#tiny',
  plugins: ['colorpicker', 'searchreplace', 'table', 'lists', 'textcolor']
});*/


app.listen(5758, () => {
    console.log('Server is running on http://localhost:5758 or http://127.0.0.1:5758');
  });