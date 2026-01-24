
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @echo $# ==================================
    @echo $# Inline File Tests (inline)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TEST=01
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TEST=02
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TEST=03
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TEST=04
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TEST=05
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t06: .SYMBOLIC
    @set TEST=06
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t07: .SYMBOLIC
    @set TEST=07
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Inline File Tests (inline) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
