plustest_name = nih

!include testenv1.mif

objs =  &
    arrayob.obj  &
    assoc.obj  &
    assocint.obj  &
    bag.obj  &
    bitboard.obj  &
    bitset.obj  &
    class.obj  &
    collecti.obj  &
    date.obj  &
    dictiona.obj  &
    float.obj  &
    fraction.obj  &
    heap.obj  &
    identdic.obj  &
    identset.obj  &
    integer.obj  &
    iterator.obj  &
    keysortc.obj  &
    link.obj  &
    linkedli.obj  &
    linkob.obj  &
    lookupke.obj  &
    nihclerr.obj  &
    nil.obj  &
    object.obj  &
    oiofd.obj  &
    oionih.obj  &
    oiostrea.obj  &
    oiotbl.obj  &
    orderedc.obj  &
    point.obj  &
    random.obj  &
    range.obj  &
    rectangl.obj  &
    seqcltn.obj  &
    set.obj  &
    sharedqu.obj  &
    sortedcl.obj  &
    stack.obj  &
    string.obj  &
    time.obj 

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags += -DNESTED_TYPES -w0 -I. -fi=__ialias.h

test_flags_386 = -mf
test_flags_i86 = -ml
test_flags_axp =

test : .symbolic start clean start_test $(objs)
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
