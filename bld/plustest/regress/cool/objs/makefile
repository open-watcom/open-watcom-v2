plustest_name = cool
!include ../../environ.mif

objs = &
   associat.obj base_bnn.obj base_bnt.obj base_has.obj base_lis.obj &
   base_m_v.obj base_mat.obj base_que.obj base_sta.obj base_vec.obj &
   bignum.obj binary_n.obj binary_t.obj bit_set.obj bit_tabl.obj &
   calendar.obj char.obj complex.obj country.obj d_node.obj date_tim.obj &
   gen_stri.obj hash_tab.obj list.obj m_vector.obj matrix.obj n_node.obj &
   n_tree.obj nt_state.obj pair.obj property.obj quaterni.obj queue.obj &
   random.obj rational.obj regexp.obj set.obj stack.obj string.obj &
   test.obj timer.obj timezone.obj value.obj vector.obj

!ifdef test1
test_cflags = -ot-op-d1
alt_error=01
!else ifdef test2
test_cflags = -ot-op-d1-xds
alt_error=02
!else ifdef test3
test_cflags = -op-d2-xds
alt_error=03
!else
test_cflags = -op-d2
alt_error=00
!endif

test_cflags += -zq -I../.. -w0
!ifdef __UNIX__
test_cflags += -fi=../__ialias.h
!endif

test_cflags_i86 = -ml -zm -s

.c: ..
.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

test : .symbolic start_test ../cool.lib
    @%make global
    %append $(log_file) PASS $(%__CWD__)

../cool.lib : $(objs) cool.lbc
    $(librarian_$(arch)) -q-n-b-p=512 $^@ @cool.lbc

cool.lbc : makefile
    %create $^@
    @for %i in ($(objs)) do @%append $^@ +%i

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
