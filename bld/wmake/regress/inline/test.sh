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
        echo \#\# INLINE \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
	exit
    fi
}

function do_err_check() {
    if [ "$?" != "0" ]; then
        echo \#\# INLINE \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
	exit
    fi
}

if [ "$2" == "" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start INLINE File Test
echo \# ===========================

TEST=1
print_header
$1 -h -f inline01 > tmp.out 2>&1
diff inline01.cmp tmp.out
do_err_check
[ -a test.1 -a -a test.2 -a -a test.3 ]
do_check

TEST=2
print_header
$1 -h -f inline02 > tmp.out 2>&1
diff inline02.cmp tmp.out
do_err_check
[ ! -a test.1 -a ! -a test.2 -a ! -a test.3 ]
do_check

TEST=3
print_header
$1 -h -f inline03 > tmp.out 2>&1
diff inline03.cmp tmp.out
do_check

TEST=4
print_header
$1 -h -f inline04 > tmp.out 2>&1
diff inline04.cmp tmp.out
do_check

# UNIX seems to behave too differently to make this test very meaningful
#TEST=5
#print_header
#$1 -h -f inline05 > tmp.out 2>&1
#diff inline05.cmp tmp.out
#do_check

TEST=6
print_header
$1 -h -f inline06 > tmp.out 2>&1
diff inline06.cmp tmp.out
do_check

TEST=7
print_header
$1 -h -f inline07 > tmp.out 2>&1
diff inline07.cmp tmp.out
do_check

rm -f tmp.out
