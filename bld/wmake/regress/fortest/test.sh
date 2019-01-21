#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   For Loop Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test successful
    else
        echo \#\# FORTEST $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# For Loop Tests
echo \# ===========================

# Need our 'rem' utility on the PATH
PATH=../cmds:$PATH

TEST=01
print_header
$1 -h -f for$TEST > test$TEST.lst 2>&1
diff for$TEST.chk test$TEST.lst
do_check

TEST=02
print_header
$1 -h -f for$TEST > test$TEST.lst 2>&1
diff for$TEST.chk test$TEST.lst
do_check

#TEST=03
#print_header
#$1 -h -f for$TEST > test$TEST.lst 2>&1
#diff for$TEST.chk test$TEST.lst
#do_check

TEST=04
print_header
echo  > test${TEST}b.lst
cat for${TEST}a.chk > test${TEST}a.lst
ls -1 >> test${TEST}a.lst
cat for${TEST}b.chk >> test${TEST}a.lst
ls -1 for?? >> test${TEST}a.lst
cat for${TEST}c.chk >> test${TEST}a.lst
$1 -h -f for$TEST > test${TEST}b.lst 2>&1
sort < test${TEST}b.lst > test${TEST}bs.lst
sort < test${TEST}a.lst > test${TEST}as.lst
diff test${TEST}bs.lst test${TEST}as.lst
do_check

TEST=05
print_header
$1 -h -f for$TEST > test$TEST.lst 2>&1
diff for$TEST.chk test$TEST.lst
do_check

TEST=06
print_header
$1 -h -f for$TEST > test$TEST.lst 2>&1
diff for$TEST.chk test$TEST.lst
do_check

TEST=07
print_header
$1 -h -f for$TEST > test$TEST.lst 2>&1
diff for$TEST.chk test$TEST.lst
do_check

TEST=08
print_header
$1 -h -f for$TEST > test$TEST.lst 2>&1
diff -b for${TEST}u.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
    rm -f *.o
fi
