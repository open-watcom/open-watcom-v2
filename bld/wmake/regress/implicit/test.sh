#!/bin/sh

ERRORS=0

usage() {
    echo usage: $0 prgname errorfile
    exit
}

print_header() {
    echo \# -----------------------------
    echo \#   Implicit Rule Test $TEST
    echo \# -----------------------------
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo \#      Test successful
    else
        echo \#\# IMPLICIT $TEST \#\# >> $LOGFILE
        echo Error: Test unsuccessful!!! | tee -a $LOGFILE
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
echo \# Implicit Rule Tests
echo \# ===========================

TEST=01
print_header
rm -f err$TEST.lst
$1 -h -l err$TEST.lst > test$TEST.lst
do_check

TEST=02
print_header
sleep 1
echo >hello.h
$1 -h -c -f imp$TEST > test$TEST.lst
diff imp$TEST.chk test$TEST.lst
do_check

TEST=03
print_header
sleep 1
echo >hello.h
$1 -c -h -f imp$TEST > test$TEST.lst
diff imp$TEST.chk test$TEST.lst
do_check

TEST=04
print_header
sleep 1
echo >hello.h
$1 -h -c -f imp$TEST -ms > test$TEST.lst
diff imp$TEST.chk test$TEST.lst
do_check

# Won't work with non-OW compilers, which may not be available on UNIX
#
TEST=05
print_header
rm -f hello.obj
$1 -f imp$TEST -h > test$TEST.lst
sed "s:of .*[\\/]:of :" test$TEST.lst | diff imp$TEST.chk -
do_check

#    hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
rm -f *.obj app.lnk app.exe hello.* hello?.*
if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
