#pmake: cpu_i86 os_win
CC = wcc
CFLAGS = -d2 -w4 -zW -zq -zu -ml -i"$(win_h)" -I"../../h" -I..
res1_flags = -bt=windows -i"../../h" -I..
name = test
CDIR = .;..\c

$(name).exe : $(name).obj $(name).res ..\wini86\rcsdll.lib
    wlink debug all name $(name) file $(name) sys windows lib { windows commdlg ..\wini86\rcsdll } op map op stack=16k
    wrc $(name).res

$(name).obj : $(name).c
    wcc $< -bt=windows $(CFLAGS)

$(name).res : $(name).rc $(resources)
    wrc -r -i=.. -i"$(win_h)" -fo=.\$(name).res $(res1_flags) $[@

clean : .SYMBOLIC
    rm -f *.obj *.exe *.map *.res *.err