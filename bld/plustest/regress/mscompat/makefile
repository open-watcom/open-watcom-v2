plustest_name = mscompat
!include ../environ.mif

files_err = &
diag0011$(ext) &
diag0085$(ext) &
diag0086$(ext) &
diag0187$(ext) &
diag0198$(ext) &
diag0251$(ext) &
diag0267$(ext) &
diag0268$(ext) &
lisa006$(ext) &
lisa012$(ext)  

files_comp = &
ms01$(ext) &
ptrfun04$(ext) &
udcnv14$(ext) &
udcnv15$(ext) &
udcnv16$(ext) &
udcnv17$(ext)

.extensions:
.extensions:  .exe .obj .merr .cpp

.cpp.exe :
    e:\cmds\mclauto.cmd $[* 
    $(run) $[*.exe >> mscompat.out
    
.cpp.merr :
    e:\cmds\mclauto.cmd $[* /c
    type $[*.merr >> mscompat.out

ext=.merr
err_out = $+ $(files_err) $-

ext=.exe
comp_out = $+ $(files_comp) $-


test : .symbolic start_test $(comp_out) $(err_out)
    diff mscompat.out mscompat.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

save : .symbolic
    @if exist mscompat.out cp mscompat.out s$(arch)_$(alt_error).sav

global : .symbolic
    @if exist *.merr rm *.merr
    @%make common_clean
