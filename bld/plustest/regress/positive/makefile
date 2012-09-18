plustest_name = positive

!include testenv.mif
!include source/positive.mif

!ifdef test1
test_flags_386 = -d2-xss-mf
test_flags_i86 = -d2-xss-mc-2
test_flags_axp = -d2-xss-si
opt_EH_yes = -xss
add_br_ = $(br_opt)
alt_error = 01
!else ifdef test2
test_flags_386 = -d1-xss-mf
test_flags_i86 = -d1-xss-mm
test_flags_axp = -d1-xss-si
opt_EH_yes = -xss
alt_error = 02
!else ifdef test3
test_flags_386 = -d2-xst-ms-zfp
test_flags_i86 = -d2-xst-ml
test_flags_axp = -d2-xst
opt_EH_yes = -xst
alt_error = 03
!else ifdef test4
test_flags_386 = -d1-xst-mf
test_flags_i86 = -d1-xst-ms
test_flags_axp = -d1-xst-si
opt_EH_yes = -xst
alt_error = 04
!else ifdef test5
test_flags_386 = -d1-xds-mf
test_flags_i86 = -d1-xds-mc-2
test_flags_axp = -d1-xds
opt_EH_yes = -xss
alt_error = 05
!else ifdef test6
test_flags_386 = -d2-xds-mf
test_flags_i86 = -d2-xds-mm
test_flags_axp = -d2-xds
opt_EH_yes = -xss
alt_error = 06
!else ifdef test7
test_flags_386 = -d1-xdt-ms-zfp
test_flags_i86 = -d1-xdt-ml
test_flags_axp = -d1-xdt
opt_EH_yes = -xst
alt_error = 07
!else ifdef test8
test_flags_386 = -d2-xdt-mf
test_flags_i86 = -d2-xdt-ms
test_flags_axp = -d2-xdt
opt_EH_yes = -xst
alt_error = 08
!else ifdef test9
test_flags_386 = -d1-xs-oe-mf
test_flags_i86 = -d1-xs-oe-ms
test_flags_axp = -d1-xs-oe
opt_EH_yes = -xs
alt_error = 09
!else ifdef test10
test_flags_386 = -d2s-xdt-mf
test_flags_i86 = -d2s-xdt-ms
test_flags_axp = -d2s-xdt
opt_EH_yes = -xst
alt_error = 10
!else ifdef test11
test_flags_386 = -d2s-xs-mf
test_flags_i86 = -d2s-xs-ms
test_flags_axp = -d2s-xs
opt_EH_yes = -xs
alt_error = 11
!else
test_flags_386 = -ox-d1-xdt-ms-zfp
test_flags_i86 = -ox-d1-xdt-ms
test_flags_axp = -ox-d1-xdt
opt_EH_yes = -xst
alt_error = 00
add_br_ = $(br_opt)
!endif

test_flags_386 += $(add_br_$(no_br_$^*))

test_flags = -we-w0 -Isource

# Some tests require more than the default stack size
stack_i86 = op stack=12k

.c: source

link3.exe : link3.obj link3b.obj
link4.exe : link4.obj link4b.obj
link5.exe : link5.obj link5b.obj
link6.exe : link6.obj link6b.obj
link7.exe : link7.obj link7b.obj
link8.exe : link8.obj link8b.obj
link9.exe : link9.obj link9b.obj
link11.exe : link11.obj link11b.obj
link14.exe : link14.obj link14b.obj link14c.obj
link15.exe : link15.obj link15b.obj link15c.obj
fnstat1.exe : fnstat1.obj fnstat1b.obj
exc01.exe : exc01.obj exc01b.obj
cltemp21.exe : cltemp21.obj cltmp21b.obj
friend4.exe : friend4.obj friend4b.obj

test : .symbolic clean start_test test.lnk $(positive_exes) ./chk_exec.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write $(%ERROR_FILE) OK
    $]@ exec.out >>$(%ERROR_FILE)
    diff -b $(%ERROR_FILE) error.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @if exist failure rm -f failure
    @%make common_clean
