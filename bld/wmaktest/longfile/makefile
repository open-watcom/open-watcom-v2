
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# Long FileName Tests (longfile)"
!else
    @echo $# Long FileName Tests (longfile)
!endif
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @%write "hello tmp.xyz" LONGFILENAME OK
    @%create hello.h
    @$(test_wmake) -h -f $@ -a > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @rm -f "hello tmp.xyz"
    @rm -f hello.h
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -a -ms -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    # This one MUST NOT use -a switch!
    @$(test_wmake) -h -f $@ -ms -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t06: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -m > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Long FileName Tests (longfile) - Test $(%TT:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
