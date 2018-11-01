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
        echo \#\# INLINE \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

do_err_check() {
    if [ "$?" -ne "0" ]; then
        echo \#\# INLINE \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start INLINE File Test
echo \# ===========================

TEST=1
print_header
$1 -h -f inline01 > test1.lst 2>&1
diff inline01.chk test1.lst
do_err_check
[ -a test.1 -a -a test.2 -a -a test.3 ]
do_check

TEST=2
print_header
$1 -h -f inline02 > test2.lst 2>&1
diff inline02.chk test2.lst
do_err_check
[ ! -a test.1 -a ! -a test.2 -a ! -a test.3 ]
do_check

TEST=3
print_header
$1 -h -f inline03 > test3.lst 2>&1
diff inline03.chk test3.lst
do_check

TEST=4
print_header
$1 -h -f inline04 > test4.lst 2>&1
diff inline04.chk test4.lst
do_check

# UNIX seems to behave too differently to make this test very meaningful
#TEST=5
#print_header
#$1 -h -f inline05 > test5.lst 2>&1
#diff -i inline05.chk test5.lst
#do_check

TEST=6
print_header
$1 -h -f inline06 > test6.lst 2>&1
diff -i inline06.chk test6.lst
do_check

TEST=7
print_header
$1 -h -f inline07 > test7.lst 2>&1
diff inline07.chk test7.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
