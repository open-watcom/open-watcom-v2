#!/bin/sh

function usage() {
    echo "usage: $0 <progname>"
    exit
}

if [ "$1" == "" ]; then 
    usage
fi

export LC_MESSAGES=en_US

rm -f error.out

# Make sure the utilities exist
cd cmds
$1 -h bootstrap=1
cd ..

# ===========================
# -- test1 - Multiple Dependents Test
# ===========================
cd test1
./test.sh $1 ../error.out
cd ..

# ===========================
# -- TEST2 - Implicit Rules Test
# ===========================
echo =================================================================
cd implicit
./test.sh $1 ../error.out
cd ..

# ===========================
# -- FORTEST - FOR LOOP TEST
# ===========================
echo =================================================================
cd fortest
./test.sh $1 ../error.out
cd ..

# ===========================
# -- PRETEST - PRE COMPILER TEST
# ===========================
echo =================================================================
cd pretest
./test.sh $1 ../error.out
cd ..

# ===========================
# -- UPDTEST - UPDATE TEST
# ===========================
echo =================================================================
cd updtest 
./test.sh $1 ../error.out
cd ..

# ===========================
# -- ERRTEST - ERROR TEST
# ===========================
echo =================================================================
echo =================================================================
echo Error tests not yet ported to Unix
echo =================================================================
cd errtest
#./test.sh $1 ../error.out
cd ..

# ===========================
# -- INLINE TEST -
# ===========================
echo =================================================================
cd inline 
./test.sh $1 ../error.out
cd ..

# ===========================
# -- PREPROCESS TEST -
# ===========================
echo =================================================================
cd preproc
./test.sh $1 ../error.out
cd ..

# ===========================
# -- MACROS TEST -
# ===========================
echo =================================================================
cd macros
./test.sh $1 ../error.out
cd ..

# ===========================
# -- MISC TEST -
# ===========================
echo =================================================================
cd misc
./test.sh $1 ../error.out
cd ..

# ===========================
# -- LONG FILENAME TEST -
# ===========================
echo =================================================================
cd longfile
./test.sh $1 ../error.out
cd ..


# ===========================
# -- End of Test
# ===========================
echo ============= DONE DONE DONE ====================

if [ -f error.out ]; then
    echo @@@@@@@@@ ERRORS FOUND @@@@@@@@@
    echo look at error.out for listing
fi
