
tests = &
    t01

!ifdef %BUILDTEST
buildtest = 1
!endif

!ifdef __UNIX__
nuldev = /dev/null
!else
nuldev = NUL
!endif

numset= &
  0001 0002 0003 0004 0005 0006 0007 0008 0009 0010 &
  0011 0012 0013 0014 0015 0016 0017 0018 0019 0020 &
  0021 0022 0023 0024 0025 0026 0027 0028 0029 0030

all: $(tests) clean .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @%create $@.lst
    @-$(test_wmake) -h -f $@ -l $@.lst > $@.lst
    @-diff -b -i $@.chk $@.lst
    @%make result

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
    # create C source files fooN..N.c
    @for %i in ($(numset)) do @%write foo%i.c extern int printf(const char *,...); extern void foo%i(void); void foo%i(void){printf("%i\t");};
    # create C source file main.c
    @%write main.c extern int printf(const char *,...);
    @for %i in ($(numset)) do @%append main.c extern void foo%i(void);
    @%append main.c int main(void)
    @%append main.c {
    @for %i in ($(numset)) do @%append main.c     foo%i();
    @%append main.c     printf( "\n" );
    @%append main.c     return( 0 );
    @%append main.c }
    # create wlink directive file
    @%write main.lnk FILE main.obj
    @for %i in ($(numset)) do @%append main.lnk FILE foo%i.obj

clean: .PROCEDURE .EXPLICIT
    @rm -f *.obj *.exe main.* foo*.c
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp
