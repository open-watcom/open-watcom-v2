
tests = &
    test01 &
    test02 &
    test03 &
    test04 &
    test05 &
    clean

.BEFORE
    @set ERRLOG=..\error.out
    @echo $# ===========================
    @echo $# Implicit Rules Tests
    @echo $# ===========================
    @%create hello.obj
    @%create hello.cpp

all: $(tests) .SYMBOLIC

test01: .SYMBOLIC
    @set TEST=01
    @%make header
    @set ERRORS=0
    @$(test_wmake) -f imp$(%TEST) -h -l err$(%TEST).lst > test$(%TEST).lst
    @%make result

test02: .SYMBOLIC
    @set TEST=02
    @%make header
    @sleep 2
    @%create hello.h
    @set ERRORS=0
    @$(test_wmake) -f imp$(%TEST) -h -c > test$(%TEST).lst
    @diff imp$(%TEST).chk test$(%TEST).lst
    @%make result

test03: .SYMBOLIC
    @set TEST=03
    @%make header
    @sleep 2
    @%create hello.h
    @set ERRORS=0
    @$(test_wmake) -f imp$(%TEST) -c -h > test$(%TEST).lst
    @diff imp$(%TEST).chk test$(%TEST).lst
    @%make result

test04: .SYMBOLIC
    @set TEST=04
    @%make header
    @sleep 2
    @%create hello.h
    @set ERRORS=0
    @$(test_wmake) -f imp$(%TEST) -h -c /ms /m > test$(%TEST).lst
    @diff imp$(%TEST).chk test$(%TEST).lst
    @%make result

test05: .SYMBOLIC
    @set TEST=05
    @%make header
    @rm hello.obj
    @set ERRORS=0
    @$(test_wmake) -f imp$(%TEST) -h > tmp$(%TEST).lst
    @sed "s:of .*[\\/]:of :" tmp$(%TEST).lst > test$(%TEST).lst
    @diff imp$(%TEST).chk test$(%TEST).lst
    @%make result

header: .PROCEDURE .EXPLICIT
    @echo $# ------------------------------
    @echo $#     Implicit Rules Test $(%TEST)
    @echo $# ------------------------------

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# IMPLICIT RULES TEST $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    #:: hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
    @rm *.obj app.lnk app.exe hello.* hello?.*
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
