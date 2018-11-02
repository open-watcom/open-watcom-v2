#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Preprocessor IF Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \# Test successful
    else
        echo \#\# PRETEST $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===================================
echo \# Preprocessor IF Tests
echo \# ===================================

TEST=01
print_header
$1 -h -f pre${TEST}a pre${TEST}a -f pre${TEST}b pre${TEST}b
do_check

TEST=02
print_header
$1 -h -f pre$TEST pre$TEST > test$TEST.lst 2>&1
diff pre$TEST.chk test$TEST.lst
do_check

TEST=03
print_header
$1 -h -f pre$TEST pre$TEST
do_check

TEST=04
print_header
$1 -h -f pre$TEST pre$TEST
do_check

TEST=05
print_header
$1 -h -f pre$TEST pre$TEST
do_check

TEST=06
print_header
$1 -h -f pre$TEST > test$TEST.lst 2>&1
diff pre$TEST.chk test$TEST.lst
do_check

TEST=07
print_header
$1 -h -f pre$TEST > test$TEST.lst 2>&1
diff pre$TEST.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
