#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \# Test $TEST successful
    else
        echo \#\# PRETEST \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===================================
echo \# Start DOPRE
echo \# ===================================

TEST=1
print_header
$1 -h -f pre01 pre01 -f pre02 pre02
do_check

TEST=2
print_header
$1 -h -f pre03 pre03 > test2.lst 2>&1
diff pre03.chk test2.lst
do_check

TEST=3
print_header
$1 -h -f pre04 pre04
do_check

TEST=4
print_header
$1 -h -f pre05 pre05
do_check

TEST=5
print_header
$1 -h -f pre06 pre06
do_check

TEST=6
print_header
$1 -h -f pre07 > test6.lst 2>&1
diff pre07.chk test6.lst
do_check

TEST=7
print_header
$1 -h -f pre08 > test7.lst 2>&1
diff pre08.chk test7.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
