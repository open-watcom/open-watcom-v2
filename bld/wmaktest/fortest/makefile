
tests = &
    t01 &
    t02 &
    t03 &
    t04 &
    t05 &
    t06 &
    t07 &
    t08 &
    clean

.BEFORE
    @set ERRLOG=../error.out
    @set CLEANUP=1
    @set ERRORS=0
    @echo $# ==================================
!ifdef __UNIX__
    @echo "$# For Loop Tests (fortest)"
!else
    @echo $# For Loop Tests (fortest)
!endif
    @echo $# ==================================

all: $(tests) .SYMBOLIC

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
    @type $@a.chk   > $@.tmp
    @dir /b /-o     >> $@.tmp
    @type $@b.chk   >> $@.tmp
    @dir /b /-o t?? >> $@.tmp
    @type $@c.chk   >> $@.tmp
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
    @%create $@.lst
    @#rem Need to set prompt, otherwise the test fails...
    @prompt $p$g
    @type $@.chk > $@.tmp
    @..\cmds\prntdir "echo a" >> $@.tmp
    @%append $@.tmp a
    @..\cmds\prntdir "echo b" >> $@.tmp
    @%append $@.tmp b
    @..\cmds\prntdir "echo c" >> $@.tmp
    @%append $@.tmp c
    @-$(test_wmake) -h -f $@ > $@.lst 2>&1
    @-diff -b $@.tmp $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# For Loop Tests (fortest) - Test $(%TESTXXX:t=) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1
    @set CLEANUP=0

clean: .SYMBOLIC
    @rm -f *.obj
    @if $(%CLEANUP) == 1 @rm -f *.lst *.tmp
