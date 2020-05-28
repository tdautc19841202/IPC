#!/bin/sh

LEDVAL=1
while [ 1 = 1 ]; do
    /tmp/update/setled.sh $LEDVAL
    if [ x"$LEDVAL" = x"1" ]; then
        LEDVAL=0
    else
        LEDVAL=1
    fi
    sleep 1
done

