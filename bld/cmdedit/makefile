cmdfiles = &
    cmdalias.$(ext) &
    cmddata.$(ext) &
    cmddir.$(ext) &
    cmdedit.$(ext) &
    cmdfile.$(ext) &
    cmdio.$(ext) &
    cmdmain.$(ext) &
    cmdsave.$(ext) &
    cmdscrn.$(ext) &
    cmdstub.$(ext) &
    cmdutil.$(ext)

ext = obj
dosobjs = $+ $(cmdfiles) $-

ext = ubj
os2zuobjs = $+ $(cmdfiles) $-

ext = 2bj
os2objs = $+ $(cmdfiles) $-

ext = cbj
os2cmdshellobjs = $+ $(cmdfiles) $-

!ifndef dopt
dopt=d1
!endif

.EXTENSIONS:
.EXTENSIONS: .exe .ubj .2bj .cbj .obj .c .h .asm

.c.obj:
        $(comp286) $^& /w3 /dDOS /oas /s /$(dopt) /zq #

.c.ubj:
        $(comp286) $^& /w3 /oxs /d1 /fo=.ubj /zu /zq #

.c.2bj:
        $(comp286) $^& /w3 /oxs /d1 /fo=.2bj /zq #

.c.cbj:
        $(comp286) $^& /w3 /oxs /d1 /fo=.cbj /zq /d__CMDSHELL__ #

.asm.obj:
        $(asm286)/t/s/zd /d_MODEL=_SMALL /d_OS=_DOS $^&; #

.asm.2bj:
        $(asm286)/t/s/zd /d_MODEL=_SMALL /d_OS=_OS2 $^&,$^&.2bj; #

.asm.cbj:
        $(asm286)/t/s/zd /d_MODEL=_SMALL /d_OS=_OS2 $^&,$^&.cbj; #

.before:
        set include=$(lang_root)\h;$(watcom_dir)\h;$(watcom_dir)\os21x_h

all: .symbolic  cmdedit.exe os2edit.dll os2edit.exe os22edit.exe
                @%null

lnk = $(ramdisk)tmp.lnk
dolink = $(linker)
#
# CMDEDIT.EXE
#
cmdobjs = dosstart.obj $(dosobjs)
cmdedit.exe: $(cmdobjs) makefile
            @%write $(lnk) debug all system dos name cmdedit option quiet,dosseg lib clibs
            @%append $(lnk) op map,verbose,symfile=cmdedit.sym
            @for %i in ($(cmdobjs)) do @%append $(lnk) file %i
            $(dolink) @$(lnk)

#
# OS2EDIT.DLL
#
cmd2objs = cmdstart.2bj $(os2zuobjs)

os2edit.dll: $(cmd2objs) makefile
            @%write $(lnk) option quiet,dosseg name os2edit lib doscalls, clibs
            @%append $(lnk) system os2 dll initinstance option manyautodata
            @%append $(lnk) option newfiles op map
            @%append $(lnk) export OS2EDIT_
            @for %i in ($(cmd2objs)) do @%append $(lnk) file %i
            $(dolink) @$(lnk)

#
# DCMDEDIT.EXE (debugging version)
#
tstobjs = dcmdedit.obj $(dosobjs)

dcmdedit.exe: $(tstobjs) makefile
            @%write $(lnk) op map system dos debug all name dcmdedit option quiet,dosseg
            @for %i in ($(tstobjs)) do @%append $(lnk) file %i
            $(dolink) @$(lnk)

#
# DOS2EDIT.EXE  (debugging version)
#
ts2objs = dcmdedit.2bj ftos.2bj $(os2objs)

dos2edit.exe: $(ts2objs) makefile
            @%write $(lnk) debug all name dos2edit system os2 pmc option quiet,dosseg
            @%append $(lnk) lib doscalls option newfiles
            @for %i in ($(ts2objs)) do @%append $(lnk) file %i
            $(dolink) @$(lnk)

#
# CMDSHELL.EXE  (feed commands to CMD.EXE)
#
cmdshellobjs = cmdshell.cbj ftos.cbj $(os2cmdshellobjs)

cmdshell.exe: $(cmdshellobjs) makefile
            @%write $(lnk) debug all name cmdshell system os2 pmc option quiet,dosseg
            @%append $(lnk) lib doscalls
            @for %i in ($(cmdshellobjs)) do @%append $(lnk) file %i
            $(dolink) @$(lnk)

#
# OS2EDIT.EXE
#

os2edit.exe:    os2edit.c
        $(comp286) os2edit /ox /d1
        $(dolink) debug all op symfile sys os2 f os2edit
#
# OS22EDIT.EXE
#
os22edit.exe:    os22edit.c
        $(comp286) os22edit /ox /d1
        $(dolink) debug all op symfile sys os2 f os22edit
