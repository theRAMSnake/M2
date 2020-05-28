const jwt = require('jsonwebtoken');
const config = require('../../config');
const pass = require("../pass");

module.exports = (req, res, next) => {
    if (!req.headers.authorization) {
      return res.status(401).end();
    }
  
    // get the last part from a authorization header string like "bearer token-value"
    const token = req.headers.authorization.split(' ')[1];
  
    // decode the token using a secret key-phrase
    return jwt.verify(token, pass, (err, decoded) => {
        
        // the 401 code is for unauthorized status
        if (err) { return res.status(401).end(); }

        const userId = decoded.sub;
    
        // check if a user exists
        if (userId != '0') {
            return res.status(401).end();
        }

        return next();
    });
};