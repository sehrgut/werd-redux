#!/bin/bash

# https://cygwin.com/ml/cygwin/2003-05/msg00670.html
awk '/^[0-9]/{print $2}' $1 | addr2line -f -e ${1%%.stackdump}
