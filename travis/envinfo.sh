#!/bin/sh
# *****************************************************************
# envinfo.sh - display environment variables
# *****************************************************************
#

env | sed -n -e '/^TRAVIS/p' -e '/^OW/p' -e '/^COVERITY/p' | sort
