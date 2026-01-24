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

work_tests = &
 t05 &
 t07 &
 t10 &
 t11 &
 t12 &
 t15 &
 t16 &
 t18 &
 t19 &
 t20 &
 t21 &
 t22 &
 t23 &
 t24 &
 t25 &
 t26 &
 t28 &
 t29a &
 t29b &
 t29c &
 t30 &
 t31a &
 t31b &
 t31c &
 t31d &
 t31e &
 t31f &
 t32 &
 t33 &
 t34a &
 t34b &
 t37a &
 t37b &
 t38 &
 t41 &
 t42 &
 t43 &
 t44a &
 t44b &
 t44c

debug_tests = &
 t27 &
 t35

tests = &
 t17 &
 $(work_tests) &
 $(debug_tests) &
 clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @echo $# ==================================
    @echo $# Error Tests (errtest)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t17: .SYMBOLIC
    @set TEST=17
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@a >  $@.lst 2>&1
    @-$(test_wmake) -h -f $@b >> $@.lst 2>&1
    @-$(test_wmake) -h -f $@c >> $@.lst 2>&1
    @-$(test_wmake) -h -f $@d >> $@.lst 2>&1
    @-$(test_wmake) -h -f $@e >> $@.lst 2>&1
    @-$(test_wmake) -h -f $@f >> $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t36: .SYMBOLIC
    @set TEST=36
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ .c.obj > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t39: .SYMBOLIC
    @set TEST=39
    @set ERRORS=0
    @head $@ -3
    @%create ditty.c
    @-$(test_wmake) -h -f $@ ditty.obj > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t40: .SYMBOLIC
    @set TEST=40
    @set ERRORS=0
    @rm -f ditty.*
    @head $@ -3
    @%create $@.tst
    @chmod +r $@.tst >$@.lst
    @-$(test_wmake) -h -a -t -f $@ >> $@.lst 2>&1
    @chmod -r $@.tst
    @rm -f $@.tst
    @diff -i $@.chk $@.lst
    @%make result

$(work_tests): .SYMBOLIC
    @set TESTXXX=$@
    @set TEST=$(%TESTXXX:t=)
    @head $@ -3
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t27: .SYMBOLIC
    @set TEST=27
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ -d > $@.tmp 2>&1
    @egrep W27 $@.tmp > $@.lst
    @diff -i $@.chk $@.lst
    @%make result

t35: .SYMBOLIC
    @set TEST=35
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ -d > $@.tmp 2>&1
    @egrep W35 $@.tmp > $@.lst
    @diff -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Error Tests (errtest) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
