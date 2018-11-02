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
        echo \#\# FORTEST \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Start FORTEST
echo \# ===========================

# Need our 'rem' utility on the PATH
PATH=../cmds:$PATH

TEST=01
print_header
$1 -h -f for01 > test01.lst 2>&1
diff for01.chk test01.lst
do_check

TEST=02
print_header
$1 -h -f for02 > test02.lst 2>&1
diff for02.chk test02.lst
do_check

#TEST=03
#print_header
#$1 -h -f for03 > test03.lst 2>&1
#diff for03.chk test03.lst
#do_check

TEST=04
print_header
cat for04a.chk > test04a.lst
ls -1 >> test04a.lst
cat for04b.chk >> test04a.lst
ls -1 for?? >> test04a.lst
cat for04c.chk >> test04a.lst
$1 -h -f for04 > test04b.lst 2>&1
sort < test04b.lst > test04bs.lst
sort < test04a.lst > test04as.lst
diff test04bs.lst test04as.lst
do_check

TEST=05
print_header
$1 -h -f for05 > test05.lst 2>&1
diff for05.chk test05.lst
do_check

TEST=06
print_header
$1 -h -f for06 > test06.lst 2>&1
diff for06.chk test06.lst
do_check

TEST=07
print_header
$1 -h -f for07 > test07.lst 2>&1
diff for07.chk test07.lst
do_check

TEST=08
print_header
$1 -h -f for08 > test08.lst 2>&1
diff -b for08u.chk test08.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
    rm -f *.o
fi
