#!/bin/sh

function usage() {
    echo usage: $0 prgname errorfile
    exit
}

function print_header() {
    echo \# -----------------------------
    echo \#   Test $TEST
    echo \# -----------------------------
}

function do_check() {
    if [ "$?" == "0" ]; then
        echo \# Test $TEST successful
    else
        echo \#\# MISC \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
	exit
    fi
}

if [ "$2" == "" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start Miscellaneous Test
echo \# ===========================

TEST=1
print_header
$1 -c -h -f misc01 > tmp.out 2>&1
diff -b misc01.cmp tmp.out
do_check

TEST=2
print_header
$1 -c -h -f misc02 > tmp.out 2>&1
diff -b misc02.cmp tmp.out
do_check

TEST=3
print_header
$1 -a -c -h -f misc03 > tmp.out 2>&1
diff -b misc03.cmp tmp.out
do_check

TEST=4
print_header
$1 -a -c -h -f misc04 > tmp.out 2>&1
diff -b misc04.cmp tmp.out
do_check

TEST=5
print_header
$1 -a -c -h -f misc05 test1 test2 test3 > tmp.out 2>&1
diff -b misc05.cmp tmp.out
do_check

TEST=6
print_header

$1 -a -c -h -f misc06u > tmp.out 2>&1
diff -b misc06u.cmp tmp.out
do_check

rm tmp.out
