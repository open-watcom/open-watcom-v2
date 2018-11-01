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
        echo \#\# PREPROCESS \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

echo \# ===========================
echo \# Start Preprocessor Test
echo \# ===========================

TEST=1
print_header
$1 -h -f prep01 > test1.lst 2>&1
diff -b -i prep01.chk test1.lst
do_check

TEST=2
print_header
$1 -h -f prep02 -m -ms > test2a.lst 2>&1
diff prep02.chk test2a.lst
do_check
$1 -h -f prep02 -m > test2b.lst 2>&1
diff prep02.chk test2b.lst
do_check

TEST=3
print_header
$1 -h -f prep03 > test3.lst 2>&1
diff -b -i prep03.chk test3.lst
do_check

TEST=4
print_header
$1 -h -f prep04 > test4.lst 2>&1
diff -b prep04.chk test4.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
