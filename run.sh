#!/bin/sh

if [ "$(uname)" == "Darwin" ]; then
    open build/pong.app
else
    cd build
    ./pong
fi
