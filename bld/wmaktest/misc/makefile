
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07

!ifdef __UNIX__
chk_file = $@u.chk
!else
chk_file = $@.chk
!endif

all: $(tests) clean .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a test1 test2 test3 > $@.lst 2>&1
    @-diff -b -i $@.chk $@.lst
    @%make result

t06 t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -c -a > $@.lst 2>&1
    @-diff -b -i $(chk_file) $@.lst
    @%make result

!include ../common.mif

.BEFORE
    @set TITLE=Miscellaneous Tests (misc)
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
!endif
    @echo $# ==================================

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%CLEANUP) == 1 @rm *.lst
