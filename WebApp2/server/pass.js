var fs = require('fs');
 
let pass = 'undefined'
try
{
    pass = fs.readFileSync('/materia/passwd', 'utf8');
}
catch (e)
{
    pass = fs.readFileSync('c:\\snake\\passwd', 'utf8');
}

module.exports = pass;