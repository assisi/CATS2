#!/bin/sh
ls -lt $1 | head -2 | tail -1 | awk '{print $9}'
