.EXTENSIONS:
.EXTENSIONS: .exp .lib .pl3 .obj .qbj .asm .psm .c

cflags = -oais -s -zq -w3

INCS = &
 386flda.inc 386fldc.inc 386fldm.inc 386fldd.inc 386ldi4.inc &
 386ldfs.inc 386fdld.inc 386i4ld.inc 386ldfd.inc 386fsld.inc &
 386fprem.inc 386sqrt.inc 386log.inc 386poly.inc &
 386trig.inc 386sind.inc 386atan.inc &
 386f2xm1.inc 386fpemu.inc 386fxam.inc 386round.inc &
 consts.inc fpubits.inc fpucc.inc memops.inc stkops.inc &
 tagtab.inc tentab.inc xception.inc


all: emudos.lib emustub.lib emu387.qnx emu387w.obj .SYMBOLIC
    @%null

emudos.lib: emu387.obj 386inite.obj
    wlib /s /t /n /b emudos +emu387.obj +386inite.obj

emustub.lib: 386stub.obj
    wlib /s /t /n /b emustub +386stub.obj

emu387.qnx: qnxemu87.qbj emu387.qbj fpeqnx.qbj
    wlink system qnx386 name $^@ file qnxemu87.qbj,emu387.qbj,fpeqnx.qbj opt priv=0,map,long,quiet,res=..\emu86\emu87.u

emu387.obj: emu387.asm $(INCS)
    masm /mx /s /dOFFSET32 /d_OS=_PLDT /dSEGMENTED /I$(mathlib_dir)\h /I$(emu386) $[@;
    wtouch compile.tim

emu387w.obj: emu387.asm $(INCS)
    masm5 /Mx /p /s /d_OS=_PLDT /d__WIN387__ /I$(mathlib_dir)\h /I$(emu386) $[@,$^.;

emu387.qbj: emu387.asm $(INCS)
    masm5 /Mx /p /s /dOFFSET32=OFFSET /dSEGMENTED /d_OS=_QNX /I$(mathlib_dir)\h /I$(emu386) $[@,$^.;
#    386asm $[@ -obj $^. -nol -twoc -d OFFSET32=OFFSET -d _OS=_QNX -d SEGMENTED -i $(mathlib_dir)\h\ -i $(emu386)\

.c : ..\emu86
.c.qbj:
    $(libcomp386) -r-fpr $[* $(cflags) -i$(new_qnx_h) -fo=.qbj -DQNX32 -bt=qnx

.psm.obj:
    # for some reason we can not execute 386asm if the command line is too long (sigh)
    #386asm $[@ -nol -twoc -d _OS=_PLDT -d SEGMENTED -i $(mathlib_dir)\h\ -i $(emu386)\ -i $(watcom_dir)\h\
    386asm $[@ -nol -twoc -d _OS=_PLDT -d SEGMENTED -i $(mathlib_dir)\h\,$(watcom_dir)\h\,$(comp_cfg_dir)\h\

386sqrt.obj: 386sqrt.psm 386sqrt.inc 386fdld.inc 386ldfd.inc xception.inc
