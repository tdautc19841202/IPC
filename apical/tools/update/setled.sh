#!/bin/sh

LEDGPIO=`cat /tmp/gpio_upd_led`

if [ ! -d /sys/class/gpio/gpio$LEDGPIO ]; then
    echo $LEDGPIO > /sys/class/gpio/export
fi
echo out      > /sys/class/gpio/gpio$LEDGPIO/direction
echo $1       > /sys/class/gpio/gpio$LEDGPIO/value
echo $LEDGPIO > /sys/class/gpio/unexport
