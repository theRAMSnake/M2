const express = require('express');
const zmq = require("zeromq")
const crypto = require('crypto');
const pass = require("../pass");
const router = new express.Router();

const sock = new zmq.Request
sock.connect("tcp://62.171.175.23:5756")

async function materiaGet(req)
{
    var key = crypto.createHash('sha256').update(pass, 'utf8').digest();

    const IV = key.subarray(0, 16);
    var plaintext = req;

    console.log(plaintext);
 
    let cipher = crypto.createCipheriv('aes-256-cbc', key, IV);
    cipher.setAutoPadding(true);
    let encrypted = cipher.update(plaintext, 'utf8', 'hex');
    encrypted += cipher.final('hex');

    let msg = Buffer.from(encrypted, 'hex');

    sock.send(msg);
    const [result] = await sock.receive();

    let decipher = crypto.createDecipheriv('aes-256-cbc', key, IV);
    decipher.setAutoPadding(true);
    let decrypted = decipher.update(result, 'binary', 'utf8');
    return (decrypted + decipher.final('utf8'));
}

router.post('/materia', (req, res) => 
{
    materiaGet(JSON.stringify(req.body)).then( (results) =>
    {
        res.status(200).json({
            message: results.toString()
        });
    });
});

module.exports = router;