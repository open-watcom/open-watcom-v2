
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07

!ifdef __UNIX__
chk_file = $@u.chk
!else
chk_file = $@.chk
!endif

.BEFORE
    @set ERRLOG=../error.out
    @set TITLE=Miscellaneous Tests (misc)
    @set CLEANUP=1
    @set ERRORS=0
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
!endif
    @echo $# ==================================

all: $(tests) clean .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a test1 test2 test3 > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t06 t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a > $@.lst 2>&1
    @-diff -b -i $(chk_file) $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# $(%TITLE) - Test $(%TT:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1
    @set CLEANUP=0

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%CLEANUP) == 1 @rm *.lst
