plustest_name = contain
# this is a long test so quit if something goes wrong
no_continue = yes
!include ../environ.mif

!ifdef test1
save_out = $(arch)_1
comp_flags = -d2-j
EH_yes = -xst
opt_386 += -ei-zp1
opt_i86 += -ei-zp2
alt_error=01
!else ifdef test2
save_out = $(arch)_2
comp_flags = -oxratl-d1-j
opt_386 += -ei-zp2
EH_yes = -xss
alt_error=02
!else ifdef test3
save_out = $(arch)_3
comp_flags = -oxratl-d1-j
EH_yes = -xs
opt_386 += -ei
opt_i86 += -ei
alt_error=03
!else
save_out = $(arch)_0
comp_flags = -d2-j
EH_yes = -xs
opt_386 += -ei-zp4
opt_i86 += -ei-zp2
alt_error=00
!endif

special_memchk = -zl -wcd=887

opt_386 += -3s-mf
opt_i86 += -3-ml -fi=source/noinline.h
opt_axp += -si

comp_flags += -w4 -we -zq -I=source $(EH_$(EH_$[&)) $(special_$[&) $(opt_$(arch))

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

ext = $(exe)

EH_wctest4 = yes
EH_wctest20 = yes
EH_wctest28 = yes
EH_wctest29 = yes
EH_wctest30 = yes
EH_wctest31 = yes
EH_wctest32 = yes
EH_wctest36 = yes
EH_wctest37 = yes
EH_wctest38 = yes
EH_wctest40 = yes
EH_wctest41 = yes
EH_wctest42 = yes
EH_wctest43 = yes
EH_wctest44 = yes
EH_wctest45 = yes
EH_wctest47 = yes
EH_wctest48 = yes
EH_wctest51 = yes
EH_wctest53 = yes
EH_wctest56 = yes
EH_wcvt2 = yes
EH_wcvt5 = yes
EH_wcvt6 = yes
EH_wcvt9 = yes
EH_wcvt12 = yes
EH_wcvt13 = yes
EH_wcvt14 = yes
EH_wcvt22 = yes
EH_wcht6 = yes
EH_wcht7 = yes
EH_wcht9 = yes
EH_wcht11 = yes
EH_wcht13 = yes
EH_wcht14 = yes
EH_wcht16 = yes
EH_wcht17 = yes
EH_wcst5 = yes
EH_wcst6 = yes
EH_wcst8 = yes
EH_wcst10 = yes
EH_wcst11 = yes
EH_wcst12 = yes
EH_wcst13 = yes
EH_wcst14 = yes
EH_wcst15 = yes

wc_list_template =     &
        wctype.$(ext)   &
        wctest.$(ext)   &
        wctest1.$(ext)  &
        wctest2.$(ext)  &
        wctest3.$(ext)  &
        wctest4.$(ext)  &
        wctest7.$(ext)  &
        wctest8.$(ext)  &
        wctest9.$(ext)  &
        wctest10.$(ext) &
        wctest11.$(ext) &
        wctest12.$(ext) &
        wctest14.$(ext) &
        wctest15.$(ext) &
        wctest16.$(ext) &
        wctest17.$(ext) &
        wctest19.$(ext) &
        wctest20.$(ext) &
        wctest21.$(ext) &
        wctest22.$(ext) &
        wctest23.$(ext) &
        wctest24.$(ext) &
        wctest26.$(ext) &
        wctest27.$(ext) &
        wctest28.$(ext) &
        wctest29.$(ext) &
        wctest30.$(ext) &
        wctest31.$(ext) &
        wctest32.$(ext) &
        wctest33.$(ext) &
        wctest34.$(ext) &
        wctest35.$(ext) &
        wctest36.$(ext) &
        wctest37.$(ext) &
        wctest38.$(ext) &
        wctest40.$(ext) &
        wctest41.$(ext) &
        wctest42.$(ext) &
        wctest43.$(ext) &
        wctest44.$(ext) &
        wctest45.$(ext) &
        wctest47.$(ext) &
        wctest48.$(ext) &
        wctest49.$(ext) &
        wctest50.$(ext) &
        wctest51.$(ext) &
        wctest52.$(ext) &
        wctest53.$(ext) &
        wctest54.$(ext) &
        wctest55.$(ext) &
        wctest56.$(ext) &
        wctest59.$(ext) &
        wctest61.$(ext) &
        wctest62.$(ext)

wc_vect_template =  &
        wcvt1.$(ext) &
        wcvt2.$(ext) &
        wcvt3.$(ext) &
        wcvt4.$(ext) &
        wcvt5.$(ext) &
        wcvt6.$(ext) &
        wcvt7.$(ext) &
        wcvt8.$(ext) &
        wcvt9.$(ext) &
        wcvt10.$(ext) &
        wcvt12.$(ext) &
        wcvt13.$(ext) &
        wcvt14.$(ext) &
        wcvt15.$(ext) &
        wcvt16.$(ext) &
        wcvt19.$(ext) &
        wcvt20.$(ext) &
        wcvt21.$(ext) &
        wcvt22.$(ext)


wc_hash_template =  &
        wcht1.$(ext) &
        wcht2.$(ext) &
        wcht3.$(ext) &
        wcht4.$(ext) &
        wcht5.$(ext) &
        wcht6.$(ext) &
        wcht7.$(ext) &
        wcht9.$(ext) &
        wcht10.$(ext) &
        wcht11.$(ext) &
        wcht12.$(ext) &
        wcht13.$(ext) &
        wcht14.$(ext) &
        wcht16.$(ext) &
        wcht17.$(ext) &
        wcht18.$(ext)
        
wc_skip_template =  &
        wcst1.$(ext) &
        wcst2.$(ext) &
        wcst3.$(ext) &
        wcst4.$(ext) &
        wcst5.$(ext) &
        wcst6.$(ext) &
        wcst7.$(ext) &
        wcst8.$(ext) &
        wcst10.$(ext) &
        wcst11.$(ext) &
        wcst12.$(ext) &
        wcst13.$(ext) &
        wcst14.$(ext) &
        wcst15.$(ext) &
        wcst16.$(ext)

wc_example_template =  &
        wcvdli.$(ext)    &
        alloctor.$(ext)

exes =   $(wc_list_template) $(wc_vect_template) $(wc_hash_template) &
                $(wc_skip_template) $(wc_example_template)

.error
    %append exec.out $(%msg)

.cpp : source
.cpp.obj:
    @set msg=*** failure to compile $[@
    $(wpp_$(arch)) $[@ $(comp_flags) -fo=.obj

.obj.$(ext):
    @set msg=*** failure to link $^@
    $(linker) @cont.lnk name $^@ file $[@
    @set msg=*** failure to execute $^@
    @%append exec.out --- start execution of $^&
    $(run) $(exec_prefix)$@ >>exec.out
    @%append exec.out --- end execution of $^&
    @rm $^&.map

test : .symbolic start_test memchk.obj cont.lnk $(exes)
    @set msg=*** output differences
    sed -f fixpath.sed exec.out | diff - exec.chk
    @set msg=*** general failure
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create exec.out
    @set msg=general error
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

cont.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch)) OPTION noredef
    @%append $^@ FILE memchk.obj

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
