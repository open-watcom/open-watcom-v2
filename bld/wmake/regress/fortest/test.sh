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
        echo \#\# FORTEST \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        exit
    fi
}

if [ "$2" == "" ]; then 
    usage
fi

LOGFILE=$2

# Need our 'rem' utility on the PATH
PATH=../cmds:$PATH

TEST=A
print_header
rm -f tst2.out
$1 -h -f for01 > tst2.out 2>&1
diff tst2.out for01.cmp
do_check

TEST=B
print_header
rm -f tst2.out
$1 -h -f for02 > tst2.out 2>&1
diff tst2.out for02.cmp
do_check

#TEST=C
#print_header
#rm -f tst2.out
#$1 -h -f for03 > tst2.out 2>&1
#diff tst2.out for03.cmp
#do_check

TEST=D
print_header
cat for04a.cmp > tmpfile.tmp
ls -1 >> tmpfile.tmp
cat for04b.cmp >> tmpfile.tmp
ls -1 for?? >> tmpfile.tmp
cat for04c.cmp >> tmpfile.tmp
rm -f tst2.out
$1 -h -f for04 > tst2.out 2>&1
sort < tst2.out > tst2.sort
sort < tmpfile.tmp > tmpfile.sort
diff tst2.sort tmpfile.sort
do_check

TEST=E
print_header
rm -f tst2.out
$1 -h -f for05 > tst2.out 2>&1
diff tst2.out for05.cmp
do_check

TEST=F
print_header
rm -f tst2.out
$1 -h -f for06 > tst2.out 2>&1
diff tst2.out for06.cmu
do_check

TEST=G
print_header
rm -f tst2.out
$1 -h -f for07 > tst2.out 2>&1
diff tst2.out for07.cmp
do_check

TEST=H
print_header
rm -f tst2.out
$1 -h -f for08 > tst2.out 2>&1
diff -b tst2.out for08.cmu
do_check


rm -f tmpfile.*
rm -f tst2.*
rm -f temp.out
rm -f *.o
