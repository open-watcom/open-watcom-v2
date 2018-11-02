#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Update Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test $1 successful
    else
        echo \#\# UPDTEST $TEST \#\# >> $LOGFILE
        echo Error: Test $1 unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Update Tests
echo \# ===========================

# Get our 'rem' version on PATH
export PATH=../cmds:$PATH

TEST=01
print_header
$1 -h -f upd$TEST
do_check a
$1 -h -f upd$TEST -m -sn -y > test$TEST.lst
do_check b
diff upd$TEST.chk test$TEST.lst
do_check c

TEST=02
print_header
$1 -h -f upd$TEST -c
do_check

TEST=03
print_header
$1 -h -f upd$TEST
do_check

TEST=04
print_header
$1 -h -f upd$TEST > test$TEST.lst 2>&1
diff -b upd$TEST.chk test$TEST.lst
do_check

TEST=05
print_header
$1 -h -s -f upd$TEST > test$TEST.lst 2>&1
diff -b upd$TEST.chk test$TEST.lst
do_check

TEST=06
print_header
$1 -h -f upd$TEST > test$TEST.lst 2>&1
diff upd$TEST.chk test$TEST.lst
do_check

TEST=07
print_header
$1 -h -f upd$TEST > test$TEST.lst 2>&1
diff upd$TEST.chk test$TEST.lst
do_check

echo \# ---------------------------
echo \#   Test 8  --- ONLY FOR DOS
echo \# ---------------------------
#TEST=08
#print_header
#$1 -h -f upd$TEST
#do_check

TEST=09
print_header
$1 -h -f upd$TEST
do_check
    
TEST=10
print_header
$1 -h -f upd$TEST
do_check
    
TEST=11
print_header
$1 -h -ms -f upd$TEST > test$TEST.lst 2>&1
diff upd$TEST.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
