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
        echo \#\# MISC \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start Miscellaneous Test
echo \# ===========================

TEST=1
print_header
$1 -c -h -f misc01 > test1.lst 2>&1
diff -b misc01.chk test1.lst
do_check

TEST=2
print_header
$1 -c -h -f misc02 > test2.lst 2>&1
diff -b misc02.chk test2.lst
do_check

TEST=3
print_header
$1 -a -c -h -f misc03 > test3.lst 2>&1
diff -b misc03.chk test3.lst
do_check

TEST=4
print_header
$1 -a -c -h -f misc04 > test4.lst 2>&1
diff -b -i misc04.chk test4.lst
do_check

TEST=5
print_header
$1 -a -c -h -f misc05 test1 test2 test3 > test5.lst 2>&1
diff -b misc05.chk test5.lst
do_check

TEST=6
print_header
$1 -a -c -h -f misc06u > test6.lst 2>&1
diff -b misc06u.chk test6.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
