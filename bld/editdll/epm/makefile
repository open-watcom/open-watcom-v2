lnk = $(%ramdisk)tmp.lnk

c_opts = /zq /w4 /bt=OS2 /i$(watcom_dir)\os220_h
!ifndef on_build_machine
c_opts += /d2
!endif

epmlink.dll : epmlink.obj
    @%create $(lnk)
    @%append $(lnk) sys os2v2 dll initinstance terminstance pm debug all
    @%append $(lnk) option description 'WATCOM Editor DLL for EPM'
    @%append $(lnk) option manyautodata, quiet, symfile, caseexact
    @%append $(lnk) libpath $(watcom_lib)
    @%append $(lnk) file epmlink.obj
    $(linker) name $^@ @$(lnk)
    wlib /n /q /b /c $^*.lib +$^*.dll
    del $(lnk)

epmlink.obj : epmlink.c
    $(comp386) $(c_opts) /bd $^*

epmtest.exe : epmtest.obj
    wlink d a op q, st=16k, case sys os2v2_pm file epmtest.obj lib epmlink.lib

epmtest.obj : epmtest.c
    $(comp386) $(c_opts) $^*

epmspy.exe : epmspy.obj
    wlink option quiet sys os2v2_pm file epmspy.obj

epmspy.obj : epmspy.c
    $(comp386) $(c_opts) $^*
