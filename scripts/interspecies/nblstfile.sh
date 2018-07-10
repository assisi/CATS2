#!/bin/sh
wc -l $1'/'$(ls -lt $1 | head -2 | tail -1 | awk '{print $9}') | awk '{print $1}'
