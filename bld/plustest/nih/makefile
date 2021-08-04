plustest_name = nih

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

!include testenv.mif

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

test : .symbolic start $(objs)
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
