#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Long File Name $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test successful
    else
        echo \#\# LONGNAME $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Long File Name Tests
echo \# ===========================

TEST=01
print_header
echo LONGFILENAME OK > "hello tmp.tmp"
echo >hello.h
$1 -h -a -f long$TEST > test$TEST.lst 2>&1
diff -b long$TEST.chk test$TEST.lst
do_check

TEST=02
rm "hello tmp.tmp"
rm hello.h
print_header
$1 -h -ms -a -f long$TEST > test$TEST.lst 2>&1
diff -b long$TEST.chk test$TEST.lst
do_check

TEST=03
print_header
# This one MUST NOT use -a switch!
$1 -h -ms -f long$TEST > test$TEST.lst 2>&1
diff -b long$TEST.chk test$TEST.lst
do_check

# Unix version uses forward slashes
TEST=04
print_header
$1 -h -m -f long${TEST}u > test$TEST.lst 2>&1
diff -b long${TEST}u.chk test$TEST.lst
do_check

TEST=05
print_header
$1 -h -m -f long$TEST > test$TEST.lst 2>&1
diff -b long$TEST.chk test$TEST.lst
do_check

# Slightly different semantics on Unix
# (case sensitive filesystem)
TEST=06
print_header
$1 -h -m -f long$TEST > test$TEST.lst 2>&1
diff -b long${TEST}u.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
