plustest_name = winnt
!include ..\environ.mif

!ifdef test1
cflags = -oxa-d1
!else ifdef test2
cflags = -oxa-d1 -br
!else ifdef test3
cflags = -d2 -br
!else
cflags = -d2
!endif

cflags += -w0-zq -i$(%watcom)\h\nt

.c.obj:
    $(wpp_$(arch)) $[@ $(cflags)
    
test : .symbolic tomexe.exe tomdll.dll
    if exist exec.out del exec.out
    $(run) tomexe
    diff exec.out exec.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

tomexe.exe : tomexe.obj tomexe.lnk
    $(linker) @$]@

tomdll.dll : tomdll.obj tomdll.lnk
    $(linker) @$]@
    
tomdll.obj : tomdll.c
    $(wpp_$(arch)) $[@ $(cflags) -bd
    
tomexe.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ DEBUG all
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=128k, quiet
    @%append $^@ FILE tomexe
    
tomdll.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ DEBUG all
    @%append $^@ SYSTEM nt dll initinstance terminstance reference __DLLstart_
    @%append $^@ OPTION map, caseexact, stack=128k, quiet
    @%append $^@ FILE tomdll
    
global : .symbolic
    @%make common_clean
