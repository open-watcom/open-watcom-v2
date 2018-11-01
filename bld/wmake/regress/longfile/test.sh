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
        echo \#\# LONGNAME \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start Long File Name Test
echo \# ===========================

TEST=1
print_header
echo LONGFILENAME OK > "hello tmp.tmp"
echo >hello.h
$1 -h -a -f long01 > test1.lst 2>&1
diff -b long01.chk test1.lst
do_check

TEST=2
rm "hello tmp.tmp"
rm hello.h
print_header
$1 -h -ms -a -f long02 > test2.lst 2>&1
diff -b long02.chk test2.lst
do_check

TEST=3
print_header
# This one MUST NOT use -a switch!
$1 -h -ms -f long03 > test3.lst 2>&1
diff -b long03.chk test3.lst
do_check

# Unix version uses forward slashes
TEST=4
print_header
$1 -h -m -f long04u > test4.lst 2>&1
diff -b long04u.chk test4.lst
do_check

TEST=5
print_header
$1 -h -m -f long05 > test5.lst 2>&1
diff -b long05.chk test5.lst
do_check

# Slightly different semantics on Unix
# (case sensitive filesystem)
TEST=6
print_header
$1 -h -m -f long06 > test6.lst 2>&1
diff -b long06u.chk test6.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
