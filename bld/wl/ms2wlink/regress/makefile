!ifndef MSWL
MSWL = ms2wlink
!endif

tests = test01 &
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

all : $(TESTS) .symbolic
    @echo ms2wlink tests PASS

test01 : .symbolic
    $(MSWL) (obj(1); | diff - $@.chk

test02 : .symbolic
    $(MSWL) obj,(exe,map); | diff - $@.chk

test03 : .symbolic
    $(MSWL) 1 2,1 2,3 4; | diff - $@.chk

test04 : .symbolic
    $(MSWL) 1+2,1+2,3+4; | diff - $@.chk

test05 : .symbolic
    $(MSWL) obj@rsp1.lnk; | diff - $@.chk

test06 : .symbolic
    $(MSWL) obj @rsp1.lnk; | diff - $@.chk

test07 : .symbolic
    $(MSWL) obj,exe@rsp1.lnk; | diff - $@.chk

test08 : .symbolic
    $(MSWL) obj,exe @rsp1.lnk; | diff - $@.chk

test09 : .symbolic
    $(MSWL) obj@rsp2.lnk; | diff - $@.chk

test10 : .symbolic
    $(MSWL) obj @rsp2.lnk; | diff - $@.chk

test11 : .symbolic
    $(MSWL) obj,exe@rsp2.lnk; | diff - $@.chk

test12 : .symbolic
    $(MSWL) obj,exe @rsp2.lnk; | diff - $@.chk

test13 : .symbolic
    $(MSWL) @rsp3.lnk; | diff - $@.chk

test14 : .symbolic
    $(MSWL) obj,@rsp3.lnk; | diff - $@.chk

test15 : .symbolic
    $(MSWL) @rsp4.lnk; | diff - $@.chk

test16 : .symbolic
    $(MSWL) @rsp4.lnk 2; | diff - $@.chk

test17 : .symbolic
    $(MSWL) @rsp4.lnk+2; | diff - $@.chk

test18 : .symbolic
    $(MSWL) @rsp6a.lnk , | diff - $@.chk

test19 : .symbolic
    $(MSWL) @rsp6b.lnk ,,, | diff - $@.chk

test20 : .symbolic
    $(MSWL) @rsp5.lnk ,,,, | diff - $@.chk

test21 : .symbolic
    $(MSWL) /SEG:100/st:3 obj,/STACK:0x400,foo/PM:VIO;/ST:123 | diff - $@.chk

test22 : .symbolic
    $(MSWL) @rsp7.lnk | diff - $@.chk

