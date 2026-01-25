
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    t09 &
    t10 &
    t11 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set ERRORS=0
    @set VER=
    @echo $# ==================================
    @echo $# Update Tests (updtest)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set VER= a
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@
    @%make result
    @set ERRORS=0
    @set VER= b
    @$(test_wmake) -h -f $@ -m -sn -y > $@.lst
    @%make result
    @set ERRORS=0
    @set VER= c
    @diff -i $@.chk $@.lst
    @%make result
    @set VER=

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ /c
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -s > $@.lst
    @diff -b -i $@.chk $@.lst
    @%make result

t06: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ > $@.lst
    @diff -i $@.chk $@.lst
    @%make result

t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -h -f $@ > $@.lst
    @diff -i $@.chk $@.lst
    @%make result

#t08: .SYMBOLIC
#    @set TT=$@
#    @set VER= a
#    @head -n 3 $@
#    @$(test_wmake) -h -f $@ >$@.lst
#    @%make result
#    @set ERRORS=0
#    @set VER= b
#    @diff -i $@.chk $@.lst
#    @%make result
#    @set VER=

t09: .SYMBOLIC
    @set TT=$@
    @head -n 3 $@
    @$(test_wmake) -h -f $@
    @%make result

t10: .SYMBOLIC
    @set TT=$@
    @head -n 3 $@
    @$(test_wmake) -h -f $@
    @%make result

t11: .SYMBOLIC
    @set TT=$@
    @head -n 3 $@
    @$(test_wmake) -h -f $@ -ms > $@.lst 2>&1
    @diff -b -i $@.chk $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test$(%VER) successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# Update Tests (updtest) - Test $(%TT:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test$(%VER) unsuccessful!!!
    @echo $# Error: Test$(%VER) unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
