#!/bin/bash
# TODO Description

set -e

sudo v4l2-ctl --set-ctrl power_line_frequency=1 -i $1

sleep 1

sudo v4l2-ctl -c focus_auto=0 -i $1

sleep 1

sudo v4l2-ctl --set-ctrl white_balance_temperature_auto=0 -i $1
