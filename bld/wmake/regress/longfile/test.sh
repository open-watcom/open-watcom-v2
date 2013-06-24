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
        echo \#\# LONGNAME \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
	exit
    fi
}

if [ "$2" == "" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start Long File Name Test
echo \# ===========================

TEST=1
print_header
echo LONGFILENAME OK > "hello tmp.tmp"
wtouch hello.h
rm -f tmp.out
$1 -h -a -f long01 > tmp.out 2>&1
diff -b long01.chk tmp.out
do_check

TEST=2
rm "hello tmp.tmp"
rm hello.h
print_header
rm tmp.out
$1 -h -ms -a -f long02 > tmp.out 2>&1
diff -b long02.chk tmp.out
do_check

TEST=3
print_header
rm tmp.out
# This one MUST NOT use -a switch!
$1 -h -ms -f long03 > tmp.out 2>&1
diff -b long03.chk tmp.out
do_check

# Unix version uses forward slashes
TEST=4
print_header
rm tmp.out
$1 -h -m -f long04u > tmp.out 2>&1
diff -b long04u.chk tmp.out
do_check

TEST=5
print_header
rm tmp.out
$1 -h -m -f long05 > tmp.out 2>&1
diff -b long05.chk tmp.out
do_check

# Slightly different semantics on Unix
# (case sensitive filesystem)
TEST=6
print_header
rm tmp.out
$1 -h -m -f long06 > tmp.out 2>&1
diff -b long06u.chk tmp.out
do_check

rm tmp.out
