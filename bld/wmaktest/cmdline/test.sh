#!/bin/sh

# *************************************************************
# A few notes: when running some of these tests, wmake may
# exit before the command line is fully processed. Hence the
# -l switch may not work and we should use stdout/stderr
# redirection to capture output. This is a sensible thing to
# do anyway because that way we know processing of the -l
# switch will not interfere with the tests in any way.
# Also note that -l only logs errors (stderr), not normal
# output (stdout). If a test needs to capture both, it has to
# use redirection.
# *************************************************************

ERRLOG=../error.out
ERRORS=0
VER=

usage() {
    echo "usage: $0 prgname errorfile"
    exit
}

do_check() {
    if [ "$?" -eq "0" ]; then
        echo "#      Test$VER successful"
    else
        echo "## Test $TEST ##" >> $ERRLOG
        echo "Error: Test$VER unsuccessful!!!" | tee -a $ERRLOG
        ERRORS=1
    fi
}

if [ -z "$1" ]; then
    usage
fi

echo "# =================================="
echo "# Command Line Error Tests (cmdline)"
echo "# =================================="

TEST=01
VER= a
head t${TEST} -3
$1 "-." > x${TEST}a.lst 2>&1
egrep Error x${TEST}a.lst > t${TEST}a.lst
diff t${TEST}a.chk t${TEST}a.lst
do_check

set VER= b
$1 "- " > x${TEST}b.lst 2>&1
egrep Error x${TEST}b.lst > t${TEST}b.lst
diff -b t${TEST}b.chk t${TEST}b.lst
do_check
set VER=

set TEST=02
head t${TEST} -3
$1 -h -f > x${TEST}.lst 2>&1
egrep Error x${TEST}.lst > t${TEST}.lst
diff t${TEST}.chk t${TEST}.lst
do_check

set TEST=03
head t${TEST} -3
$1 -h "-" 2> t${TEST}.lst
$1 -h - 2>> t${TEST}.lst
$1 -h "-\" 2>> t${TEST}.lst
diff t${TEST}.chk t${TEST}.lst
do_check

# if [ exist *.obj ] rm -f *.obj
if [ "$ERRORS" -eq "0" ]; then
    rm -f *.lst
fi
