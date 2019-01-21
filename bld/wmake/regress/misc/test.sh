#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Miscellaneous Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test successful
    else
        echo \#\# MISC $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Miscellaneous Tests
echo \# ===========================

TEST=01
print_header
$1 -c -h -f misc$TEST > test$TEST.lst 2>&1
diff -b misc$TEST.chk test$TEST.lst
do_check

TEST=02
print_header
$1 -c -h -f misc$TEST > test$TEST.lst 2>&1
diff -b misc$TEST.chk test$TEST.lst
do_check

TEST=03
print_header
$1 -a -c -h -f misc$TEST > test$TEST.lst 2>&1
diff -b misc$TEST.chk test$TEST.lst
do_check

TEST=04
print_header
$1 -a -c -h -f misc$TEST > test$TEST.lst 2>&1
diff -b -i misc$TEST.chk test$TEST.lst
do_check

TEST=05
print_header
$1 -a -c -h -f misc$TEST test1 test2 test3 > test$TEST.lst 2>&1
diff -b misc$TEST.chk test$TEST.lst
do_check

TEST=06
print_header
$1 -a -c -h -f misc${TEST}u > test$TEST.lst 2>&1
diff -b misc${TEST}u.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
