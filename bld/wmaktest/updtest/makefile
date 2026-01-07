
tests = &
    test01 &
    test02 &
    test03 &
    test04 &
    test05 &
    test06 &
    test07 &
    test09 &
    test10 &
    test11 &
    clean

.BEFORE
    @set ERRLOG=..\error.out
    @echo $# ===========================
    @echo $# Update Tests
    @echo $# ===========================
    @%create hello.obj
    @%create hello.cpp

all: $(tests) .SYMBOLIC

test01: .SYMBOLIC
    @set TEST=01
    @%make header
    @set VER=a
    @$(test_wmake) -h -f upd$(%TEST)
    @%make result
    @set VER=b
    @$(test_wmake) -h -f upd$(%TEST) -m -sn -y > test$(%TEST).lst
    @%make result
    @set VER=c
    @diff upd$(%TEST).chk test$(%TEST).lst
    @%make result

test02: .SYMBOLIC
    @set TEST=02
    @%make header
    @set VER=
    @$(test_wmake) -h -f upd$(%TEST) /c
    @%make result

test03: .SYMBOLIC
    @set TEST=03
    @%make header
    @set VER=
    @$(test_wmake) -h -f upd$(%TEST)
    @%make result

test04: .SYMBOLIC
    @set TEST=04
    @%make header
    @$(test_wmake) -h -f upd$(%TEST) > test$(%TEST).lst 2>&1
    @set VER=
    @diff -b upd$(%TEST).chk test$(%TEST).lst
    @%make result

test05: .SYMBOLIC
    @set TEST=05
    @%make header
    @$(test_wmake) -h -s -f upd$(%TEST) > test$(%TEST).lst
    @set VER=
    @diff -b upd$(%TEST).chk test$(%TEST).lst
    @%make result

test06: .SYMBOLIC
    @set TEST=06
    @%make header
    @$(test_wmake) -h -f upd$(%TEST) > test$(%TEST).lst
    @set VER=
    @diff upd$(%TEST).chk test$(%TEST).lst
    @%make result

test07: .SYMBOLIC
    @set TEST=07
    @%make header
    @$(test_wmake) -h -f upd$(%TEST) > test$(%TEST).lst
    @set VER=
    @diff upd$(%TEST).chk test$(%TEST).lst
    @%make result

#test08: .SYMBOLIC
#    @set TEST=08
#    @%make header
#    @set VER=a
#    @$(test_wmake) -h -f upd$(%TEST) >test$(%TEST).lst
#    @%make result
#    @set VER=b
#    @diff upd$(%TEST).chk test$(%TEST).lst
#    @%make result

test09: .SYMBOLIC
    @set TEST=09
    @%make header
    @set VER=
    @$(test_wmake) -h -f upd$(%TEST)
    @%make result

test10: .SYMBOLIC
    @set TEST=10
    @%make header
    @set VER=
    @$(test_wmake) -h -f upd$(%TEST)
    @%make result

test11: .SYMBOLIC
    @set TEST=11
    @%make header
    @$(test_wmake) -h -ms -f upd$(%TEST) > test$(%TEST).lst 2>&1
    @set VER=
    @diff -b upd$(%TEST).chk test$(%TEST).lst
    @%make result

header: .PROCEDURE .EXPLICIT
    @echo $# ------------------------------
    @echo $#  Update Test $(%TEST)
    @echo $# ------------------------------

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test $(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# UPDTEST $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test $(%VER) unsuccessful!!!
    @echo $# Error: Test $(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
