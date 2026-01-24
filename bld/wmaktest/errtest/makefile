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

tests = &
    test17 &
    test27 &
    test35 &
    clean

list1 = 05 07 10 11 12 15 16 18 19 20 21 22 23 24 25 26 28 29a 29b 29c 30 31a 31b 31c 31d 31e 31f 32 33 34a 34b 37a 37b 38 41 42 43 44a 44b 44c

.BEFORE
    @set ERRLOG=../error.out
    @echo $# ==================================
    @echo $# Error Tests (errtest)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

test17: .SYMBOLIC
    set TEST=17
    @set VER=
    head err$(%TEST) -3
    @$(test_wmake) -h -f err$(%TEST)a >  test$(%TEST).lst 2>&1
    @$(test_wmake) -h -f err$(%TEST)b >> test$(%TEST).lst 2>&1
    @$(test_wmake) -h -f err$(%TEST)c >> test$(%TEST).lst 2>&1
    @$(test_wmake) -h -f err$(%TEST)d >> test$(%TEST).lst 2>&1
    @$(test_wmake) -h -f err$(%TEST)e >> test$(%TEST).lst 2>&1
    @$(test_wmake) -h -f err$(%TEST)f >> test$(%TEST).lst 2>&1
    diff -i err$(%TEST).chk test$(%TEST).lst
    %make result

test27: .SYMBOLIC
    set TEST=27
    @set VER=
    head err$(%TEST) -3
    @$(test_wmake) -f err$(%TEST) -h -d > tmp$(%TEST).lst 2>&1
    egrep W$(%TEST) tmp$(%TEST).lst > test$(%TEST).lst
    diff -i err$(%TEST).chk test$(%TEST).lst
    call :result

test35: .SYMBOLIC
    set TEST=35
    @set VER=
    head err$(%TEST) -3
    @$(test_wmake) -f err$(%TEST) -h -d > tmp$(%TEST).lst 2>&1
    egrep W$(%TEST) tmp$(%TEST).lst > test$(%TEST).lst
    diff -i err$(%TEST).chk test$(%TEST).lst
    call :result

test36: .SYMBOLIC
    set TEST=36
    @set VER=
    head err$(%TEST) -3
    @$(test_wmake) -h -f err$(%TEST) .c.obj > test$(%TEST).lst 2>&1
    diff -i err$(%TEST).chk test$(%TEST).lst
    %make result

test39: .SYMBOLIC
    set TEST=39
    @set VER=
    head err$(%TEST) -3
    echo. >ditty.c
    @$(test_wmake) -h -f err$(%TEST) ditty.obj > test$(%TEST).lst 2>&1
    diff -i err$(%TEST).chk test$(%TEST).lst
    %make result

test40: .SYMBOLIC
    set TEST=40
    del ditty.*
    head err$(%TEST) -3
    echo. >err$(%TEST).tst
    chmod +r err$(%TEST).tst >test$(%TEST).lst
    @$(test_wmake) -h -a -t -f err$(%TEST) >> test$(%TEST).lst 2>&1
    chmod -r err$(%TEST).tst
    del err$(%TEST).tst
    diff -i err$(%TEST).chk test$(%TEST).lst
    %make result

    #for %%i in (05 07 10 11 12 15 16 18 19 20 21 22 23 24 25 26 28 29a 29b 29c 30 31a 31b 31c 31d 31e 31f 32 33 34a 34b 37a 37b 38 41 42 43 44a 44b 44c) DO #call :work %%i
    #for %%j in (27 35) DO %make debug %%j

work: .PROCEDURE .EXPLICIT
    set TEST=%1
    @set VER=
    head err$(%TEST) -3
    @$(test_wmake) -f err$(%TEST) -h > test$(%TEST).lst 2>&1
    diff -i err$(%TEST).chk test$(%TEST).lst
    %make result

header: .PROCEDURE .EXPLICIT
    @echo $# ------------------------------
    @echo $#  Update Test $(%TEST)
    @echo $# ------------------------------

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test $(%VER) successful

resulta: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test $(%VER) successful

resultb: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test $(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# UPDTEST $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test $(%VER) unsuccessful!!!
    @echo $# Error: Test $(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
