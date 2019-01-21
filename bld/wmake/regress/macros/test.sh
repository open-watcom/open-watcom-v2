#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Macro Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test $1 successful
    else
        echo \#\# MACRO $TEST \#\# >> $LOGFILE
        echo Error: Test $1 unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2
echo \# ===========================
echo \# Macro Tests
echo \# ===========================

TEST=01
print_header
$1 -ms -h -f macro$TEST > test$TEST.lst 2>&1
diff -b macro$TEST.chk test$TEST.lst
do_check

TEST=02
print_header
$1 -h -f macro$TEST > test${TEST}a.lst 2>&1
diff -b macro${TEST}a.chk test${TEST}a.lst
do_check a

TEST=02
print_header
$1 -h -ms -f macro$TEST > test${TEST}b.lst 2>&1
diff -b macro${TEST}b.chk test${TEST}b.lst
do_check b

TEST=03
print_header
$1 -h -f macro$TEST > test${TEST}a.lst 2>&1
diff -b macro${TEST}a.chk test${TEST}a.lst
do_check a

TEST=03
print_header
export fubar=test depends on this environment variable
$1 -h -ms -f macro$TEST > test${TEST}b.lst 2>&1
unset fubar
diff -b -i macro${TEST}b.chk test${TEST}b.lst
do_check b

TEST=04
print_header
$1 -h -f macro$TEST > test${TEST}a.lst 2>&1
diff -b macro${TEST}a.chk test${TEST}a.lst
do_check a

TEST=04
print_header
$1 -h -ms -f macro$TEST > test${TEST}b.lst 2>&1
diff -b macro${TEST}b.chk test${TEST}b.lst
do_check b

TEST=05
print_header
echo >hello.boo
echo >hello.tmp
echo >hello.c
$1 -h -ms -f macro${TEST}u > test$TEST.lst 2>&1
diff -b macro${TEST}u.chk test$TEST.lst
do_check
rm hello.boo hello.tmp hello.c

TEST=06
print_header
echo >hello.obj
echo >hello2.obj
echo >hello.c
echo >hello2.c
$1 -h -ms -f macro$TEST -a > test$TEST.lst 2>&1
diff -b macro$TEST.chk test$TEST.lst
do_check

TEST=07
print_header
echo >hello.obj
echo >hello2.obj
echo >hello.c
echo >hello2.c
$1 -h -ms -m -f macro$TEST -a cc=wcl386 > test$TEST.lst 2>&1
diff -b macro$TEST.chk test$TEST.lst
do_check

TEST=08
print_header
$1 -h -f macro$TEST > test$TEST.lst 2>&1
diff -b macro$TEST.chk test$TEST.lst
do_check

rm -f hello.obj hello2.obj hello.boo hello.tmp hello.c hello2.c 

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
