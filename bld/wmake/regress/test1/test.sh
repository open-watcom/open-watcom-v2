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

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Multiple Dependents Test
echo \# ===========================

TEST=1
$1 -h -f create
echo >err1.lst
$1 -h -f maketst1 -l err1.lst > test1.lst
diff -b tst1.chk test1.lst
diff -b err1.chk err1.lst
do_check

if [ "$ERRORS" -eq "0" ]; then
    rm -f *.obj
    rm -f *.lst
    rm -f main.*
    rm -f foo*.c
    rm -f maketst1
fi
