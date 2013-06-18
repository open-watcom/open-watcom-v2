#!/bin/sh

function usage() {
    echo usage: $0 prgname errorfile
    exit
}

function print_header() {
    echo \# -----------------------------
    echo \#   Test $TEST
    echo \# -----------------------------
}

function do_check() {
    if [ "$?" == "0" ]; then
        echo \# Test $TEST successful
    else
        echo \#\# UPDTEST \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
	exit
    fi
}

if [ "$2" == "" ]; then 
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
$1 -h -f upd01 -m -sn -y > tmp.out
do_check
diff upd01.chk tmp.out
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
$1 -h -f upd04 > tmp.out 2>&1
diff -b upd04.chk tmp.out
do_check

TEST=5
print_header
$1 -h -s -f upd05 > tmp.out 2>&1
diff -b upd05.chk tmp.out
do_check

TEST=6
print_header
$1 -h -f upd06 > tmp.out 2>&1
diff upd06.chk tmp.out
do_check

TEST=7
print_header
$1 -h -f upd07 > tmp.out 2>&1
diff upd07.chk tmp.out
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
$1 -h -ms -f upd11 > tmp.out 2>&1
diff upd11.chk tmp.out
do_check

rm -f tmp.out
