#!/bin/sh

OWVERBOSE=0
ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -------------------------------
    echo \#   Multiple Dependents Test $TEST
    echo \# -------------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test $1 successful
    else
        echo \#\# INLINE $TEST \#\# >> $LOGFILE
        echo Error: Test $1 unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Multiple Dependents Tests
echo \# ===========================

TEST=01
print_header
$1 -h -f create
echo >err$TEST.ref
echo >err$TEST.lst
$1 -h -f maketst1 -l err$TEST.lst > test$TEST.lst
diff -b err$TEST.ref err$TEST.lst
do_check a
diff -b test$TEST.chk test$TEST.lst
do_check b

rm -f *.obj
rm -f main.*
rm -f foo*.c
if [ "$ERRORS" -eq "0" ]; then
    rm -f *.ref
    rm -f *.lst
    rm -f maketst1
fi
