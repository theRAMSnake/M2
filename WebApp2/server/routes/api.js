const express = require('express');
const crypto = require('crypto');
const pass = require("../pass");
const router = new express.Router();
var Mutex = require('async-mutex').Mutex;
const logger = require('./logger');
const http = require("http")


function m4req(op, cb, errCb)
{
    const options = {
      host: 'ramsnake.net',
      port: 5754,
      path: '/api',
      method: 'POST',
      headers: {
        'Content-Type': 'text/html',
        'Content-Length': op.length
      }
    };
    const req = http.request(options, resp => {
        let output = ''
        resp.on('data', (chunk) => {
          output += chunk;
        });

        resp.on('end', () => {
            cb(output);
        });

    });

    req.on('error', (e) => {
      console.error(`problem with request: ${e.message}`);
    });

    req.write(op);
    req.end();
}

function materiaGet(req, cb)
{   
    var key = crypto.createHash('sha256').update(pass, 'utf8').digest();

    const IV = key.subarray(0, 16);
    var plaintext = req;
 
    let cipher = crypto.createCipheriv('aes-256-cbc', key, IV);
    cipher.setAutoPadding(true);
    let encrypted = cipher.update(plaintext, 'utf8', 'hex');
    encrypted += cipher.final('hex');

    let msg = Buffer.from(encrypted, 'hex').toString('base64');
    
    m4req(msg, (result)=>{
        result = Buffer.from(result, 'base64');
        let decipher = crypto.createDecipheriv('aes-256-cbc', key, IV);
        decipher.setAutoPadding(true);
        let decrypted = decipher.update(result, 'binary', 'utf8');
        let encoded = (decrypted + decipher.final('utf8'));
        cb(encoded);

    }, (err)=>{});
}

const mutex = new Mutex();
router.post('/materia', (req, res) => 
{
    mutex
    .acquire()
    .then(function(release) {
        materiaGet(JSON.stringify(req.body), (results) =>
        {
            release();
            res.status(200).json({
                message: results.toString()
            });
        });
    });    
});

module.exports = router;
