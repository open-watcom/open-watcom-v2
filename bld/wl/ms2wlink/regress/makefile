!ifndef MSWL
MSWL = ms2wlink
!endif

tests = &
    test01 &
    test02 &
    test03 &
    test04 &
    test05 &
    test06 &
    test07 &
    test08 &
    test09 &
    test10 &
    test11 &
    test12 &
    test13 &
    test14 &
    test15 &
    test16 &
    test17 &
    test18 &
    test19 &
    test20 &
    test21 &
    test22 &
    test23 &

all : $(TESTS) .SYMBOLIC
    @echo ms2wlink tests PASS

test01 : .SYMBOLIC
    $(MSWL) (obj(1); >$@.out
    diff $@.out $@.chk

test02 : .SYMBOLIC
    $(MSWL) obj,(exe,map); >$@.out
    diff $@.out $@.chk

test03 : .SYMBOLIC
    $(MSWL) 1 2,1 2,3 4; >$@.out
    diff $@.out $@.chk

test04 : .SYMBOLIC
    $(MSWL) 1+2,1+2,3+4; >$@.out
    diff $@.out $@.chk

test05 : .SYMBOLIC
    $(MSWL) obj@rsp1.lnk; >$@.out
    diff $@.out $@.chk

test06 : .SYMBOLIC
    $(MSWL) obj @rsp1.lnk; >$@.out
    diff $@.out $@.chk

test07 : .SYMBOLIC
    $(MSWL) obj,exe@rsp1.lnk; >$@.out
    diff $@.out $@.chk

test08 : .SYMBOLIC
    $(MSWL) obj,exe @rsp1.lnk; >$@.out
    diff $@.out $@.chk

test09 : .SYMBOLIC
    $(MSWL) obj@rsp2.lnk; >$@.out
    diff $@.out $@.chk

test10 : .SYMBOLIC
    $(MSWL) obj @rsp2.lnk; >$@.out
    diff $@.out $@.chk

test11 : .SYMBOLIC
    $(MSWL) obj,exe@rsp2.lnk; >$@.out
    diff $@.out $@.chk

test12 : .SYMBOLIC
    $(MSWL) obj,exe @rsp2.lnk; >$@.out
    diff $@.out $@.chk

test13 : .SYMBOLIC
    $(MSWL) @rsp3.lnk; >$@.out
    diff $@.out $@.chk

test14 : .SYMBOLIC
    $(MSWL) obj,@rsp3.lnk; >$@.out
    diff $@.out $@.chk

test15 : .SYMBOLIC
    $(MSWL) @rsp4.lnk; >$@.out
    diff $@.out $@.chk

test16 : .SYMBOLIC
    $(MSWL) @rsp4.lnk 2; >$@.out
    diff $@.out $@.chk

test17 : .SYMBOLIC
    $(MSWL) @rsp4.lnk+2; >$@.out
    diff $@.out $@.chk

test18 : .SYMBOLIC
    $(MSWL) @rsp6a.lnk , >$@.out
    diff $@.out $@.chk

test19 : .SYMBOLIC
    $(MSWL) @rsp6b.lnk ,,, >$@.out
    diff $@.out $@.chk

test20 : .SYMBOLIC
    $(MSWL) @rsp5.lnk ,,,, >$@.out
    diff $@.out $@.chk

test21 : .SYMBOLIC
    $(MSWL) /SEG:100/st:3 obj,/STACK:0x400,foo/PM:VIO;/ST:123 >$@.out
    diff $@.out $@.chk

test22 : .SYMBOLIC
    $(MSWL) @rsp7.lnk >$@.out
    diff $@.out $@.chk

test23 : .SYMBOLIC
    $(MSWL) /XLX obj,,,,def8 >$@.out
    diff $@.out $@.chk

clean : .SYMBOLIC
    rm -f *.out
