#!/bin/bash
export NODE_OPTIONS=--openssl-legacy-provider
nohup npm start &
nohup npm run bundle &
