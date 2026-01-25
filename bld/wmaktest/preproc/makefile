
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @set VER=
    @echo $# ==================================
    @echo $# Preprocessor Tests (preproc)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @set VER= a
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -m -ms > $@a.lst 2>&1
    @%make result
    @set ERRORS=0
    @set VER= b
    @$(test_wmake) -h -f $@ -m > $@b.lst 2>&1
    @diff -i $@.chk $@b.lst
    @%make result
    @set VER=

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test$(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Preprocessor Tests (preproc) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test$(%VER) unsuccessful!!!
    @echo $# Error: Test$(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
