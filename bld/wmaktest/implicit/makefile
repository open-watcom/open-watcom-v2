
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set TITLE=Implicit Rules Tests (implicit)
    @set CLEANUP=1
    @set ERRORS=0
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
!endif
    @echo $# ==================================
    @%create hello.obj
    @%create hello.cpp

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -l $@.tmp > $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @sleep 2
    @%create hello.h
    @-$(test_wmake) -h -f $@ -c > $@.lst
    @-diff $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @sleep 2
    @%create hello.h
    @-$(test_wmake) -h -f $@ -c > $@.lst
    @-diff $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @sleep 2
    @%create hello.h
    @-$(test_wmake) -h -f $@ -ms -m -c > $@.lst
    @-diff $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @rm -f hello.obj
    @-$(test_wmake) -h -f $@ > $@.tmp
    @sed "s:of .*[\\/]:of :" $@.tmp > $@.lst
    @-diff $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# $(%TITLE) - Test $(%TT:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1
    @set CLEANUP=0

clean: .SYMBOLIC
    @rm *.obj app.lnk app.exe hello.* hello?.*
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp
