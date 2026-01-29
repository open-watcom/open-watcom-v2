
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    clean

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff -i $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @set VER= a
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ -m -ms > $@a.lst 2>&1
    @%make result
    @set ERRORS=0
    @set VER= b
    @-$(test_wmake) -h -f $@ -m > $@b.lst 2>&1
    @-diff -i $@.chk $@b.lst
    @%make result
    @set VER=

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff -i $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff -i $@.chk $@.lst
    @%make result

!include ../common.mif

.BEFORE
    @set TITLE=Preprocessor Tests (preproc)
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
