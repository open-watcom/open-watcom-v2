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
        echo \#\# MACRO \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2
echo \# ===========================
echo \# Start Macro Tests
echo \# ===========================

TEST=1
print_header
$1 -ms -h -f macro01 > test1.lst 2>&1
diff -b macro01.chk test1.lst
do_check

TEST=2A
print_header
$1 -h -f macro02 > test2a.lst 2>&1
diff -b macro02a.chk test2a.lst
do_check

TEST=2B
print_header
$1 -h -ms -f macro02 > test2b.lst 2>&1
diff -b macro02b.chk test2b.lst
do_check

TEST=3A
print_header
$1 -h -f macro03 > test3a.lst 2>&1
diff -b macro03a.chk test3a.lst
do_check

TEST=3B
print_header
export fubar=test depends on this environment variable
$1 -h -ms -f macro03 > test3b.lst 2>&1
unset fubar
diff -b -i macro03b.chk test3b.lst
do_check

TEST=4A
print_header
$1 -h -f macro04 > test4a.lst 2>&1
diff -b macro04a.chk test4a.lst
do_check

TEST=4B
print_header
$1 -h -ms -f macro04 > test4b.lst 2>&1
diff -b macro04b.chk test4b.lst
do_check

TEST=5
print_header
echo >hello.boo
echo >hello.tmp
echo >hello.c
$1 -h -ms -f macro05u > test5.lst 2>&1
diff -b macro05u.chk test5.lst
do_check
rm hello.boo hello.tmp hello.c

TEST=6
print_header
echo >hello.obj
echo >hello2.obj
echo >hello.c
echo >hello2.c
$1 -h -ms -f macro06 -a > test6.lst 2>&1
diff -b macro06.chk test6.lst
do_check

TEST=7
print_header
echo >hello.obj
echo >hello2.obj
echo >hello.c
echo >hello2.c
$1 -h -ms -m -f macro07 -a cc=wcl386 > test7.lst 2>&1
diff -b macro07.chk test7.lst
do_check

TEST=8
print_header
$1 -h -f macro08 > test8.lst 2>&1
diff -b macro08.chk test8.lst
do_check

rm -f hello.obj hello2.obj hello.boo hello.tmp hello.c hello2.c 

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
