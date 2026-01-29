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
CLEANUP=1
ERRORS=0
VER=

if [ -z "$1" ]; then
    echo "usage: $0 prgname errorfile"
    exit
fi

TITLE=Command Line Error Tests (cmdline)

do_check() {
    if [ "$?" -eq "0" ]; then
        echo "#      Test${VER} successful"
    else
        echo "## ${TITLE} ${TT:t=} ##" >> $ERRLOG
        echo "Error: Test${VER} unsuccessful!!!" | tee -a $ERRLOG
        ERRORS=1
        CLEANUP=0
    fi
}

echo "# ============================="
echo "# ${TITLE}"
echo "# ============================="

TT=t01
VER=" a"
ERRORS=0
head -n 3 ${TT}
$1 "-." > ${TT}a.tmp 2>&1
egrep Error ${TT}a.tmp > ${TT}a.lst
diff ${TT}a.chk ${TT}a.lst
do_check

VER=" b"
ERRORS=0
$1 "- " > ${TT}b.tmp 2>&1
egrep Error ${TT}b.tmp > ${TT}b.lst
diff -b ${TT}b.chk ${TT}b.lst
do_check
VER=

TT=t02
ERRORS=0
head -n 3 ${TT}
$1 -h -f > ${TT}.tmp 2>&1
egrep Error ${TT}.tmp > ${TT}.lst
diff ${TT}.chk ${TT}.lst
do_check

TT=t03
ERRORS=0
head -n 3 ${TT}
$1 -h "-" 2> ${TT}.lst
$1 -h - 2>> ${TT}.lst
$1 -h "-\" 2>> ${TT}.lst
diff ${TT}.chk ${TT}.lst
do_check

rm -f *.obj
if [ "$CLEANUP" -eq "1" ]; then
    rm -f *.lst *.tmp
fi
