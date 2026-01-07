#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Inline File Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test successful
    else
        echo \#\# INLINE $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

do_err_check() {
    if [ "$?" -ne "0" ]; then
        echo \#\# INLINE $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Inline File Tests
echo \# ===========================

TEST=01
print_header
$1 -h -f inline$TEST > test$TEST.lst 2>&1
diff inline$TEST.chk test$TEST.lst
do_err_check
[ -f test.1 -a -f test.2 -a -f test.3 ]
do_check

TEST=02
print_header
$1 -h -f inline$TEST > test$TEST.lst 2>&1
diff inline$TEST.chk test$TEST.lst
do_err_check
[ ! -f test.1 -a ! -f test.2 -a ! -f test.3 ]
do_check

TEST=03
print_header
$1 -h -f inline$TEST > test$TEST.lst 2>&1
diff inline$TEST.chk test$TEST.lst
do_check

TEST=04
print_header
$1 -h -f inline$TEST > test$TEST.lst 2>&1
diff inline$TEST.chk test$TEST.lst
do_check

# UNIX seems to behave too differently to make this test very meaningful
#TEST=05
#print_header
#$1 -h -f inline$TEST > test$TEST.lst 2>&1
#diff -i inline$TEST.chk test$TEST.lst
#do_check

TEST=06
print_header
$1 -h -f inline$TEST > test$TEST.lst 2>&1
diff -i inline$TEST.chk test$TEST.lst
do_check

TEST=07
print_header
$1 -h -f inline$TEST > test$TEST.lst 2>&1
diff inline$TEST.chk test$TEST.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
