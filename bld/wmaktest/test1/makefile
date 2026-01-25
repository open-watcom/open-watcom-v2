
tests = &
    t01

.BEFORE
    @set ERRLOG=../error.out
    @set VER=
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# Multiple Dependents Tests (test1)"
!else
    @echo $# Multiple Dependents Tests (test1)
!endif
    @echo $# ==================================
    @wmake -h -f create

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set VER= a
    @set ERRORS=0
    @head -n 3 $@
    @%create $@.ref
    @%create $@.tmp
    @-$(test_wmake) -h -f maketst1 -l $@.tmp > $@.lst
    @diff -b $@.ref $@.tmp
    @%make result
    #
    @set ERRORS=0
    @set VER= b
    @diff -b $@.chk $@.lst > nul
    @%make result
    @set VER=
    #
    #@set ERRORS=0
    #@set VER= c
    #diff -b $@a.chk $@a.lst > nul
    #%make result
    @%make clean

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test$(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Multiple Dependents Tests (test1) - Test $(%TT:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test$(%VER) unsuccessful!!!
    @echo $# Error: Test$(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .PROCEDURE .EXPLICIT
    @rm -f *.obj *.exe main.* foo*.c
    @if $(%ERRORS) == 0 @rm -f *.lst *.tmp *.ref maketst1
