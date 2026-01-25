
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
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -ms -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @set VER= a
    @head -n 3 $@
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
    @set TT=$@
    @set ERRORS=0
    @set VER= a
    @head -n 3 $@
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
    @set TT=$@
    @set ERRORS=0
    @set VER= a
    @head -n 3 $@
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
    @set TT=$@
    @set ERRORS=0
    @%create hello.boo
    @%create hello.xyz
    @%create hello.c
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -ms -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result
    @rm -f hello.boo hello.xyz hello.c

t06: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @%create hello.obj
    @%create hello2.obj
    @%create hello.c
    @%create hello2.c
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -ms -m -a > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result
    @rm -f hello.obj hello2.obj hello.c hello2.c

t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @%create hello.obj
    @%create hello2.obj
    @%create hello.c
    @%create hello2.c
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -ms -m -a cc=wcl386 > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result
    @rm -f hello.obj hello2.obj hello.c hello2.c

t08: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
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
