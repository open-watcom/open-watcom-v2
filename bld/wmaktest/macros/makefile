
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    t08 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @set VER=
    @echo $# ==================================
    @echo $# Macro Tests (macros)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TEST=01
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ -ms -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TEST=02
    @set ERRORS=0
    @set VER= a
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@a.lst 2>&1
    @diff -b -i $@a.chk $@a.lst
    @%make result
    @set ERRORS=0
    @set VER= b
    @$(test_wmake) -h -f $@ -ms -m > $@b.lst 2>&1
    @diff -b -i $@b.chk $@b.lst
    @%make result
    @set VER=

t03: .SYMBOLIC
    @set TEST=03
    @set ERRORS=0
    @set VER= a
    @head $@ -3
    @-$(test_wmake) -h -f $@ > $@a.lst 2>&1
    @diff -b -i $@a.chk $@a.lst
    @%make result
    @set ERRORS=0
    @set VER= b
    @set fubar=test depends on this environment variable
    @-$(test_wmake) -h -f $@ -ms -m > $@b.lst 2>&1
    @set fubar=
    @diff -b -i $@b.chk $@b.lst
    @%make result
    @set VER=

t04: .SYMBOLIC
    @set TEST=04
    @set ERRORS=0
    @set VER= a
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@a.lst 2>&1
    @diff -b -i $@a.chk $@a.lst
    @%make result
    @set ERRORS=0
    @set VER= b
    @$(test_wmake) -h -f $@ -ms -m > $@b.lst 2>&1
    @diff -b -i $@b.chk $@b.lst
    @%make result
    @set VER=

t05: .SYMBOLIC
    @set TEST=05
    @set ERRORS=0
    @%create hello.boo
    @%create hello.tmp
    @%create hello.c
    @head $@ -3
    @$(test_wmake) -h -f $@ -ms -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result
    @rm -f hello.boo hello.tmp hello.c

t06: .SYMBOLIC
    @set TEST=06
    @set ERRORS=0
    @%create hello.obj
    @%create hello2.obj
    @%create hello.c
    @%create hello2.c
    @head $@ -3
    @$(test_wmake) -h -f $@ -ms -m -a > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result
    @rm -f hello.obj hello2.obj hello.c hello2.c

t07: .SYMBOLIC
    @set TEST=07
    @set ERRORS=0
    @%create hello.obj
    @%create hello2.obj
    @%create hello.c
    @%create hello2.c
    @head $@ -3
    @$(test_wmake) -h -f $@ -ms -m -a cc=wcl386 > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result
    @rm -f hello.obj hello2.obj hello.c hello2.c

t08: .SYMBOLIC
    @set TEST=08
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test$(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Macro Tests (macros) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test$(%VER) unsuccessful!!!
    @echo $# Error: Test$(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
