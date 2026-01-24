
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    t09 &
    t10 &
    t11 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @set VER=
    @echo $# ==================================
    @echo $# Update Tests (updtest)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TEST=01
    @set VER= a
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@
    @%make result
    @set ERRORS=0
    @set VER= b
    @$(test_wmake) -h -f $@ -m -sn -y > $@.lst
    @%make result
    @set ERRORS=0
    @set VER= c
    @diff -i $@.chk $@.lst
    @%make result
    @set VER=

t02: .SYMBOLIC
    @set TEST=02
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ /c
    @%make result

t03: .SYMBOLIC
    @set TEST=03
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@
    @%make result

t04: .SYMBOLIC
    @set TEST=04
    @set ERRORS=0
    @head $@ -3
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TEST=05
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -s -f $@ > $@.lst
    @diff -b -i $@.chk $@.lst
    @%make result

t06: .SYMBOLIC
    @set TEST=06
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst
    @diff -i $@.chk $@.lst
    @%make result

t07: .SYMBOLIC
    @set TEST=07
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst
    @diff -i $@.chk $@.lst
    @%make result

#t08: .SYMBOLIC
#    @set TEST=08
#    @set VER= a
#    @head $@ -3
#    @$(test_wmake) -h -f $@ >$@.lst
#    @%make result
#    @set ERRORS=0
#    @set VER= b
#    @diff -i $@.chk $@.lst
#    @%make result
#    @set VER=

t09: .SYMBOLIC
    @set TEST=09
    @head $@ -3
    @$(test_wmake) -h -f $@
    @%make result

t10: .SYMBOLIC
    @set TEST=10
    @head $@ -3
    @$(test_wmake) -h -f $@
    @%make result

t11: .SYMBOLIC
    @set TEST=11
    @head $@ -3
    @$(test_wmake) -h -ms -f $@ > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test$(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Update Tests (updtest) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test$(%VER) unsuccessful!!!
    @echo $# Error: Test$(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
