
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @echo $# ==================================
    @echo $# Implicit Rules Tests (implicit)
    @echo $# ==================================
    @%create hello.obj
    @%create hello.cpp

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head $@ -3
    @$(test_wmake) -h -f $@ -l x$(%TEST).lst > $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head $@ -3
    @sleep 2
    @%create hello.h
    @$(test_wmake) -h -f $@ -c > $@.lst
    @diff $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head $@ -3
    @sleep 2
    @%create hello.h
    @$(test_wmake) -h -f $@ -c > $@.lst
    @diff $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head $@ -3
    @sleep 2
    @%create hello.h
    @$(test_wmake) -h -f $@ -c /ms /m > $@.lst
    @diff $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head $@ -3
    @rm -f hello.obj
    @$(test_wmake) -h -f $@ > x$(%TEST).lst
    @sed "s:of .*[\\/]:of :" x$(%TEST).lst > $@.lst
    @-diff $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Implicit Rules Tests (implicit) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    #:: hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
    @rm *.obj app.lnk app.exe hello.* hello?.*
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
