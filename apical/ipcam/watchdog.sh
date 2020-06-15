#!/bin/sh

while [ 1 = 1 ]; do
    killall -0 ipcam
    if [ $? -ne 0 ]; then
        ipcam &
    fi

    sleep 2
done

