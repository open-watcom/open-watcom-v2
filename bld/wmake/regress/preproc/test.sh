#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Preprocessor Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test $1 successful
    else
        echo \#\# PREPROCESS $TEST \#\# >> $LOGFILE
        echo Error: Test $1 unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

echo \# ===========================
echo \# Preprocessor Tests
echo \# ===========================

TEST=01
print_header
$1 -h -f prep$TEST > test$TEST.lst 2>&1
diff -b -i prep$TEST.chk test$TEST.lst
do_check

TEST=02
print_header
$1 -h -f prep$TEST -m -ms > test${TEST}a.lst 2>&1
diff prep$TEST.chk test${TEST}a.lst
do_check a
$1 -h -f prep$TEST -m > test${TEST}b.lst 2>&1
diff prep$TEST.chk test${TEST}b.lst
do_check b

TEST=03
print_header
$1 -h -f prep$TEST > test$TEST.lst 2>&1
diff -b -i prep$TEST.chk test$TEST.lst
do_check

TEST=04
print_header
$1 -h -f prep$TEST > test$TEST.lst 2>&1
diff -b prep$TEST.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
