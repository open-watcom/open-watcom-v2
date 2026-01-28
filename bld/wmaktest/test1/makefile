
tests = &
    t01

!ifdef __UNIX__
nuldev = /dev/null
!else
nuldev = NUL
!endif

.BEFORE
    @set ERRLOG=../error.out
    @set TITLE=Multiple Dependents Tests (test1)
    @set CLEANUP=1
    @set ERRORS=0
    @set VER=
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
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
    @-diff -b $@.ref $@.tmp
    @%make result
    #
    @set ERRORS=0
    @set VER= b
    @-diff -b $@.chk $@.lst > $(nuldev)
    @%make result
    @set VER=
    #
    #@set ERRORS=0
    #@set VER= c
    #diff -b $@a.chk $@a.lst > $(nuldev)
    #%make result
    @%make clean

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test$(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# $(%TITLE) - Test $(%TT:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test$(%VER) unsuccessful!!!
    @echo $# Error: Test$(%VER) unsuccessful!!!!
    @set ERRORS=1
    @set CLEANUP=0

clean: .PROCEDURE .EXPLICIT
    @rm -f *.obj *.exe main.* foo*.c
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp *.ref maketst1
