
tests = &
    test01

.BEFORE
    @set ERRLOG=..\error.out
    @echo $# ===========================
    @echo $# Multiple Dependents Tests
    @echo $# ===========================
    @$(test_wmake) -f create -h

all: $(tests) .SYMBOLIC

test01: .SYMBOLIC
    @set TEST=01
    @%make header
    @%create err$(%TEST).ref
    @%create err$(%TEST).lst
    @set ERRORS=0
    @set VER=a
    @$(test_wmake) -h -f maketst1 -l err$(%TEST).lst > test$(%TEST).lst
    @diff -b err$(%TEST).ref err$(%TEST).lst
    @%make result
    @set ERRORS=0
    @set VER=b
    @diff -b test$(%TEST).chk test$(%TEST).lst > nul
    @%make result
    #@set ERRORS=0
    #@set VER=c
    # diff -b test$(%TEST)a.chk test$(%TEST)a.lst > nul
    # %make result
    @%make clean

header: .PROCEDURE .EXPLICIT
    @echo $# ------------------------------
    @echo $#   Multiple Dependents Test $@
    @echo $# ------------------------------

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test $(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# TEST1 $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test $(%VER) unsuccessful!!!
    @echo $# Error: Test $(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .PROCEDURE .EXPLICIT
    @rm *.obj *.exe main.* foo*.c
    @if $(%ERRORS) == 0 @rm *.ref *.lst maketst1

