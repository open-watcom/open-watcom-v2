#
# EMU87 makefile
#
.EXTENSIONS:
.EXTENSIONS: .exe .obj .2bj .qbj .rbj .asm .c
cflags = -oais -s -zq -w3
incfiles = &
 fpe86.inc e86fsld.inc e86ldfs.inc   &
 e86ldfd.inc e86fdld.inc e86i4ld.inc e86ldi8.inc e86i8ld.inc &
 e86ldi4.inc e86flda.inc e86fldc.inc e86fldm.inc e86fldd.inc &
 e86fprem.inc e86poly.inc e86log.inc e86round.inc &
 e86sqrt.inc e86atan.inc sindata.inc e86sin.inc e86f2xm1.inc &
 memops.new stkops.new

.asm.obj:
    masm /s/mx/e /I$(emu386)\ /I$(mathlib_dir)\h\ /I$(comp_cfg_dir)\h\ $[@,$^.;

.asm.qbj:
    masm /s/mx/e /I$(emu386)\ /I$(mathlib_dir)\h\ $[@,$^. /DQNX16 /D__PROTMODE__;

.asm.rbj:
    masm /s/mx/e /I$(emu386)\ /I$(mathlib_dir)\h\ $[@,$^. /DQNX32 /D__PROTMODE__;

.c.obj:
    $(libcomp286) -r-fpr $[* $(cflags)

.c.qbj:
    $(libcomp286) -r-fpr $[* $(cflags) -i$(new_qnx_h) -fo=.qbj -DQNX16 -bt=QNX

.c.rbj:
    $(libcomp286) -r-fpr $[* $(cflags) -i$(new_qnx_h) -fo=.rbj -DQNX32 -bt=QNX

all: .SYMBOLIC noemu87.lib emu87.lib emuos2.lib emuwin.lib &
                emu87_16.qnx emu87_32.qnx emu87.qnx
    @%null

noemu87.lib: emustub.obj
    wlib /n /b noemu87 +emustub

emu87.lib: initemu.obj emu8087.obj
    wlib /n /b emu87 +initemu +emu8087

emuos2.lib: initemu2.obj emu8087.2bj
    wlib /n /b emuos2 +initemu2 +emu8087.2bj

emu87_16.qnx: qnxemu87.qbj emu8087.qbj fpeqnx.qbj
    wlink system qnx name $^@ file qnxemu87.qbj,emu8087.qbj,fpeqnx.qbj opt priv=0,map,long,quiet,res=emu87.u

emu87_32.qnx: qnxemu87.rbj emu8087.rbj fpeqnx.rbj
    wlink system qnx name $^@ file qnxemu87.rbj,emu8087.rbj,fpeqnx.rbj opt priv=0,map,long,quiet,res=emu87.u

emu87.qnx : qemu87.qbj
    wlink system qnx name $^@ file qemu87.qbj opt map,quiet,res=emu87.u

emuwin.lib: initemuw.obj
    wlib /n /b emuwin +initemuw

emu8087.qbj: emu8087.asm $(incfiles)

emu8087.rbj: emu8087.asm $(incfiles)

emu8087.2bj: emu8087.asm $(incfiles)
    masm /s/mx /I$(emu386)\ /I$(mathlib_dir)\h\ /d__PROTMODE__ $[*,$[&.2bj;

emu8087.obj: emu8087.asm $(incfiles)
    masm /s/mx /I$(emu386)\ /I$(mathlib_dir)\h\ $[*;

initemu.obj: initemu.asm
    masm /s/mx /I$(emu386)\ /I$(mathlib_dir)\h\ /I$(comp_cfg_dir)\h\ $[*;
