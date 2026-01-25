
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
    @set ERRLOG=..\error.out
    @set ERRORS=0
    @echo $# ==================================
    @echo $# For Loop Tests (fortest)
    @echo $# ==================================

all: $(tests) .SYMBOLIC

t01: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff $@.chk $@.lst
    @%make result

t02: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff $@.chk $@.lst
    @%make result

t03: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff $@.chk $@.lst
    @%make result

t04: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @%create $@.lst
    @type $@a.chk > x$(%TEST).lst
    @dir /b /-o       >> x$(%TEST).lst
    @type $@b.chk >> x$(%TEST).lst
    @dir /b /-o t?? >> x$(%TEST).lst
    @type $@c.chk >> x$(%TEST).lst
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff x$(%TEST).lst $@.lst
    @%make result

t05: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff $@.chk $@.lst
    @%make result

t06: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff $@.chk $@.lst
    @%make result

t07: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff $@.chk $@.lst
    @%make result

t08: .SYMBOLIC
    @set TT=$@
    @set ERRORS=0
    @head -n 3 $@
    @%create $@.lst
    @#rem Need to set prompt, otherwise the test fails...
    @prompt $p$g
    @type $@.chk > x$(%TEST).lst
    @..\cmds\prntdir "echo a" >> x$(%TEST).lst
    @%append x$(%TEST).lst a
    @..\cmds\prntdir "echo b" >> x$(%TEST).lst
    @%append x$(%TEST).lst b
    @..\cmds\prntdir "echo c" >> x$(%TEST).lst
    @%append x$(%TEST).lst c
    @$(test_wmake) -f $@ -h > $@.lst 2>&1
    @diff -b x$(%TEST).lst $@.lst
    @%make result

result: .PROCEDURE .EXPLICIT
    @if errorlevel 1 @%make resulterr
    @if $(%ERRORS) == 0 @echo $#        Test successful

resulterr: .PROCEDURE .EXPLICIT
    @%append $(%ERRLOG) $#$# For Loop Tests (fortest) - Test $(%TEST) $#$#
    @%append $(%ERRLOG) $# Error: Test unsuccessful!!!
    @echo $# Error: Test unsuccessful!!!!
    @set ERRORS=1

clean: .SYMBOLIC
    @if exist *.obj @rm *.obj
    @if $(%ERRORS) == 0 @rm *.lst
