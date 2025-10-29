
tests = &
    test01 &
    test02 &
    test03 &
    test04 &
    test05 &
    test06 &
    test07 &
    test08 &
    clean

.BEFORE
    @set ERRLOG=..\error.out
    @echo $# ===========================
    @echo $# For Loop Tests
    @echo $# ===========================
    @%create hello.obj
    @%create hello.cpp

all: $(tests) .SYMBOLIC

test01: .SYMBOLIC
    @set TEST=01
    @%make header
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff for$(%TEST).chk for$(%TEST).lst
    @%make result

test02: .SYMBOLIC
    @set TEST=02
    @%make header
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff for$(%TEST).chk for$(%TEST).lst
    @%make result

test03: .SYMBOLIC
    @set TEST=03
    @%make header
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff for$(%TEST).chk for$(%TEST).lst
    @%make result

test04: .SYMBOLIC
    @set TEST=04
    @%make header
    %create for$(%TEST).lst
    type for$(%TEST)a.chk > tmp$(%TEST).lst
    dir /b /-o       >> tmp$(%TEST).lst
    type for$(%TEST)b.chk >> tmp$(%TEST).lst
    dir /b /-o for?? >> tmp$(%TEST).lst
    type for$(%TEST)c.chk >> tmp$(%TEST).lst
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff tmp$(%TEST).lst for$(%TEST).lst
    @%make result

test05: .SYMBOLIC
    @set TEST=05
    @%make header
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff for$(%TEST).chk for$(%TEST).lst
    @%make result

test06: .SYMBOLIC
    @set TEST=06
    @%make header
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff for$(%TEST).chk for$(%TEST).lst
    @%make result

test07: .SYMBOLIC
    @set TEST=07
    @%make header
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff for$(%TEST).chk for$(%TEST).lst
    @%make result

test08: .SYMBOLIC
    @set TEST=08
    @%make header
    %create for$(%TEST).lst
    #rem Need to set prompt, otherwise the test fails...
    prompt $p$g
    type for$(%TEST).chk > tmp$(%TEST).lst
    ..\cmds\prntdir "echo a" >> tmp$(%TEST).lst
    %append tmp$(%TEST).lst a
    ..\cmds\prntdir "echo b" >> tmp$(%TEST).lst
    %append tmp$(%TEST).lst b
    ..\cmds\prntdir "echo c" >> tmp$(%TEST).lst
    %append tmp$(%TEST).lst c
    @$(test_wmake) -f for$(%TEST) -h > for$(%TEST).lst 2>&1
    @set ERRORS=0
    @diff -b tmp$(%TEST).lst for$(%TEST).lst
    @%make result

header: .PROCEDURE .EXPLICIT
    @echo $# ------------------------------
    @echo $#  For Loop Test $(%TEST)
    @echo $# ------------------------------

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# FORTEST $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
