
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    clean

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

!include ../common.mif

.BEFORE
    @set TITLE=Implicit Rules Tests (implicit)
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
!endif
    @echo $# ==================================
    @%create hello.obj
    @%create hello.cpp

clean: .SYMBOLIC
    @rm *.obj app.lnk app.exe hello.* hello?.*
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp
