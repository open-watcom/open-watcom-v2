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
        echo \#\# UPDTEST \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start UPDTEST
echo \# ===========================

# Get our 'rem' version on PATH
export PATH=../cmds:$PATH

TEST=1
print_header
$1 -h -f upd01
do_check
$1 -h -f upd01 -m -sn -y > test1.lst
do_check
diff upd01.chk test1.lst
do_check

TEST=2
print_header
$1 -h -f upd02 -c
do_check

TEST=3
print_header
$1 -h -f upd03
do_check

TEST=4
print_header
$1 -h -f upd04 > test4.lst 2>&1
diff -b upd04.chk test4.lst
do_check

TEST=5
print_header
$1 -h -s -f upd05 > test5.lst 2>&1
diff -b upd05.chk test5.lst
do_check

TEST=6
print_header
$1 -h -f upd06 > test6.lst 2>&1
diff upd06.chk test6.lst
do_check

TEST=7
print_header
$1 -h -f upd07 > test7.lst 2>&1
diff upd07.chk test7.lst
do_check

echo \# ---------------------------
echo \#   Test 8  --- ONLY FOR DOS
echo \# ---------------------------

TEST=9
print_header
$1 -h -f upd09
do_check
    
TEST=10
print_header
$1 -h -f upd10
do_check
    
TEST=11
print_header
$1 -h -ms -f upd11 > test11.lst 2>&1
diff upd11.chk test11.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
