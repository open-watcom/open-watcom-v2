plustest_name = bitset

!ifdef test1
test_flags_386 = -d2-4r
test_flags_i86 = -d2
test_flags_axp = -d2
alt_error=01
!else
test_flags_386 = -ox-d1-4s
test_flags_i86 = -ox-d1
test_flags_axp = -ox-d1
alt_error=00
!endif

test_flags_386 += -j-mf-4r
test_flags_i86 += -j-ml
test_flags_axp += -j

!include testenv.mif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

opt_link = FILE tbitset.obj

test : .symbolic start test.lnk tbitset.obj bitset.$(exe)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean

