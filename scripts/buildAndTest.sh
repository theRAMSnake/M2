#!/bin/bash
cd ../build && make; cd -
cd ../bin && ./test; cd -
