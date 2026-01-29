
tests = &
    t01

!ifdef __UNIX__
nuldev = /dev/null
!else
nuldev = NUL
!endif

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set VER= a
    @set ERRORS=0
    @head -n 3 $@
    @%create $@.ref
    @%create $@.tmp
    @-$(test_wmake) -h -f maketst1 -l $@.tmp > $@.lst
    @-diff -b $@.ref $@.tmp
    @%make result
    #
    @set ERRORS=0
    @set VER= b
    #@-diff -b $@.chk $@.lst > $(nuldev)
    @-diff -b $@.chk $@.lst
    @%make result
    @set VER=
    #
    #@set ERRORS=0
    #@set VER= c
    #diff -b $@a.chk $@a.lst > $(nuldev)
    #%make result
    @%make clean

!include ../common.mif

.BEFORE
    @set TITLE=Multiple Dependents Tests (test1)
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# $(%TITLE)"
!else
    @echo $# $(%TITLE)
!endif
    @echo $# ==================================
    @wmake -h -f create

clean: .PROCEDURE .EXPLICIT
    @rm -f *.obj *.exe main.* foo*.c
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp *.ref maketst1
