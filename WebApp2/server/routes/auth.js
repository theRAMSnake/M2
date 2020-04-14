const express = require('express');
const passport = require('passport');

const router = new express.Router();

router.post('/login', (req, res, next) => {
    if(!req.body.password)
    {
        return res.status(400).json({
            success: false,
            message: 'error'
        });
    }

    return passport.authenticate('local', (err, user, info) => {
        if (err) 
        {
            return res.status(400).json({
                success: false,
                message: err.message
            });
        }
      
        return res.status(200).json({
            success: true,
            message: 'You have successfully logged in!',
            token: info
        });
    })(req, res, next);
});

module.exports = router;