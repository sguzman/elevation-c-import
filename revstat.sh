#!/bin/sh

# the dd command can be used to get the current stream
# position with `kill -usr1`
7z x -so $1 | dd of=pipe | egrep '^    (<title>|<revision>)'
