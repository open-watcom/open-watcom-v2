
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    t08

all: $(tests) clean .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @%create $@.lst
    @cat $@a.chk   > $@.tmp
    @dir /b /-o     >> $@.tmp
    @cat $@b.chk   >> $@.tmp
    @dir /b /-o t?? >> $@.tmp
    @cat $@c.chk   >> $@.tmp
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.tmp $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.chk $@.lst
    @%make result

t06: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.chk $@.lst
    @%make result

t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff $@.chk $@.lst
    @%make result

t08: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
!ifdef __UNIX__
    @-diff -b $@u.chk $@.lst
!else
    @-diff -b $@.chk $@.lst
!endif
    @%make result

!include ../common.mif

.BEFORE
    @set TITLE=For Loop Tests (fortest)
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
!endif
    @echo $# ==================================

clean: .SYMBOLIC
    @rm -f *.obj
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp
