
tests = &
    test01 &
    test02 &
    test03 &
    test04 &
    test05 &
    test06 &
    test07 &
    clean

.BEFORE
    @set ERRLOG=..\error.out
    @echo $# ===========================
    @echo $# Preprocessor IF Tests
    @echo $# ===========================
    @%create hello.obj
    @%create hello.cpp

all: $(tests) .SYMBOLIC

test01: .SYMBOLIC
    @set TEST=01
    @%make header
    @set VER=a
    #:: 2>nul to hide debug-build, memory-tracking diagnostic
    @$(test_wmake) -h -f pre$(%TEST)a pre$(%TEST)a -f pre$(%TEST)b pre$(%TEST)b 2>&1
    @%make result

test02: .SYMBOLIC
    @set TEST=02
    @%make header
    @$(test_wmake) -h -f pre$(%TEST) pre$(%TEST) > test$(%TEST).lst 2>&1
    @set VER=
    @diff -i pre$(%TEST).chk test$(%TEST).lst
    @%make result

test03: .SYMBOLIC
    @set TEST=03
    @%make header
    @set VER=
    @$(test_wmake) -h -f pre$(%TEST) pre$(%TEST)
    @%make result

test04: .SYMBOLIC
    @set TEST=04
    @%make header
    @set VER=
    @$(test_wmake) -h -f pre$(%TEST) pre$(%TEST)
    @%make result

test05: .SYMBOLIC
    @set TEST=05
    @%make header
    @set VER=
    @$(test_wmake) -h -f pre$(%TEST) pre$(%TEST)
    @%make result

test06: .SYMBOLIC
    @set TEST=06
    @%make header
    @$(test_wmake) -h -f pre$(%TEST) > test$(%TEST).lst 2>&1
    @set VER=
    @diff -i pre$(%TEST).chk test$(%TEST).lst
    @%make result

test07: .SYMBOLIC
    @set TEST=07
    @%make header
    @set VER=a
    @$(test_wmake) -h -f pre$(%TEST) > test$(%TEST).lst 2>&1
    @%make result
    @set VER=b
    @diff pre$(%TEST).chk test$(%TEST).lst
    @%make result

header: .PROCEDURE .EXPLICIT
    @echo $# ------------------------------
    @echo $#  Preprocessor IF Test $(%TEST)
    @echo $# ------------------------------

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test $(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# PRETEST $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test $(%VER) unsuccessful!!!
    @echo $# Error: Test $(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
