MODEL = F
!ifdef comp386
WCCEXE=$(comp386)
!else
WCCEXE=wcc386p
!endif
WCPPEXE=wcpp386
INCLUDES=/I$(watcom_dir)\os220_h
FORM=os2v2

!include ..\makefile

sample2.$(EXE) : sample.obj sampdial.obj ..\$(LIBNAME).lib
    %create sample.lnk
    %append sample.lnk  system os2v2
    %append sample.lnk  option map
    %append sample.lnk  name sample2.exe
    %append sample.lnk  debug all
    %append sample.lnk  file sample
    %append sample.lnk  file sampdial
    %append sample.lnk  library ..\$(LIBNAME)
    set lib=$(watcomlib);$(%lib)
    wlinkp @sample.lnk
