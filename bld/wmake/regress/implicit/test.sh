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
        echo \#\# IMPLICIT \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        ERRORS=1
    fi
}

if [ -z "$2" ]; then 
    usage
fi

LOGFILE=$2

echo >hello.obj
echo >hello.cpp

echo \# ===========================
echo \# Implicit Rules Test
echo \# ===========================

TEST=1
print_header
rm -f err1.lst
$1 -h -l err1.lst > test1.lst
do_check

TEST=2A
print_header
sleep 1
echo >hello.h
$1 -h -c -f imp02a > test2a.lst
diff imp02.chk test2a.lst
do_check

TEST=2B
print_header
sleep 1
echo >hello.h
$1 -c -h -f imp02b > test2b.lst
diff imp02.chk test2b.lst
do_check

TEST=2C
print_header
sleep 1
echo >hello.h
$1 -h -c -f imp02c -ms > test2c.lst
diff imp02.chk test2c.lst
do_check

# Won't work with non-OW compilers, which may not be available on UNIX
#
TEST=2D
print_header
rm -f hello.obj
$1 -f imp02d -h > test2d.lst
sed "s:of .*[\\/]:of :" test2d.lst | diff imp02d.chk -
do_check

if [ "$ERRORS" -eq "0" ]; then
#    hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
    rm -f *.obj app.lnk app.exe hello.* hello?.*
    rm -f *.lst
fi
