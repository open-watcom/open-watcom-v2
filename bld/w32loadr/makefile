!ifdef tools_10_5
ZZ = /zz
!else
ZZ =
!endif
exes =  w32bind.exe &
        w32bindnt.exe &
        os2ldr.exe &
        x32run.exe &
        tntrun.exe &
        d4grun.exe &
        ntrunner.exe

all : $(exes) .symbolic
   @%null

w32bind.exe : w32bind.c
  wcl386 w32bind /ox /zq /"option stub=wstubq"

w32bindnt.exe : w32bind.c
  wcl386 w32bind /bt=NT /l=nt /fe=w32bindnt /ox

os2ldr.exe : cstrtos2.obj loader.obj int21.obj int21os2.obj tnk.obj os2stub.exe
  *wlink system os2v2 pmc name os2ldr file cstrtos2,loader,int21,int21os2,tnk lib os2386 opt map,newfile,stack=32k,stub=os2stub.exe

dpmi_ldr.exe : loader16.obj dpmi_ldr.obj int21win.obj
  wlink system dos name dpmi_ldr file loader16,dpmi_ldr,int21win option map

os2stub.exe : loader16.obj dpmildr.obj int21win.obj
  wlink system dos name os2stub file loader16,dpmildr,int21win option map
dpmildr.obj : dpmildr.asm
        masm /mx /s dpmildr;
dpmi_ldr.obj : dpmi_ldr.asm
        masm /mx /s dpmi_ldr;
loader16.obj : loader16.asm
        masm /mx /s loader16;
os2stub.obj : os2stub.asm
        masm /mx /s os2stub;
loader.obj : loader.c loader.h
        wcc386 loader /ox /d1 /d__OS2 /d__CALL21__ /zq /i$(watcom_os220_h)
bexpand.obj : bexpand.asm
        masm /mx /s bexpand;
int21os2.obj : int21os2.c
        wcc386 int21os2 /ox /d1 /zq /i$(watcom_os220_h)
tnk.obj : tnk.asm
        masm /mx /s tnk;
int21.obj : int21.asm
        masm /mx /s int21;
cstrtos2.obj : cstrtos2.asm
        masm /mx /s cstrtos2;
cstrtwnt.obj : cstrtwnt.asm
        masm /mx /s cstrtwnt;

dbgntrun.exe : ntrun.obj cstrtwnt.obj int21.obj int21nt.obj ntstub.exe
  @%write tmp.lnk debug all
  @%append tmp.lnk sys nt name dbgntrun file cstrtwnt,ntrun,int21,int21nt opt map,stack=64k,stub=ntstub.exe
  @%append tmp.lnk lib nt
! ifdef tools_10_1
  @%append tmp.lnk lib kernel32,user32,gdi32,comdlg32,winspool,advapi32,shell32
! endif
  wlink @tmp.lnk

ntrun.exe : ntrun.obj cstrtwnt.obj int21.obj int21nt.obj ntstub.exe
  @%write tmp.lnk sys nt name ntrun file cstrtwnt,ntrun,int21,int21nt opt map,stack=64k,stub=ntstub.exe
  @%append tmp.lnk lib nt
! ifdef tools_10_1
  @%append tmp.lnk lib kernel32,user32,gdi32,comdlg32,winspool,advapi32,shell32
! endif
  wlink @tmp.lnk
  del tmp.lnk
  for %i in ( $(nt_exes) ) do copy ntrun.exe nt\%i

ntrunner.exe : ntrunner.obj cstrtwnt.obj int21.obj int21nt.obj ntstub.exe
  @%write tmp.lnk sys nt name ntrunner.exe file cstrtwnt, ntrunner,int21,int21nt opt map,stack=64k,stub=ntstub.exe
  @%append tmp.lnk lib nt
! ifdef tools_10_1
  @%append tmp.lnk lib kernel32,user32,gdi32,comdlg32,winspool,advapi32,shell32
! endif
  wlink @tmp.lnk
  del tmp.lnk

ntstub.exe : ntstub.obj
  wlink system dos file ntstub option map

ntstub.obj : ntstub.asm
        masm /mx /s ntstub;
int21nt.obj : int21nt.c
  wcc386 int21nt $(ZZ) /ox /d1 /bt=NT /zq /i$(watcom_nt_h)
ntrun.obj : loader.c loader.h
  wcc386 loader $(ZZ) /ox /d1 /bt=NT /d__NT /d__CALL21__ /fo=ntrun.obj /zq /i$(watcom_nt_h)
ntrunner.obj : loader.c loader.h
  wcc386 loader $(ZZ) /ox /d1 /bt=NT /d__NT /d__CALL21__ /dW32RUN /fo=ntrunner.obj /zq /i$(watcom_nt_h)

# we don't need to link w386run with wstub.exe
# this will make it about 14k smaller
d4grun.exe : d4grun.obj cmain32.obj int21dos.obj
   wlink system dos4g name w386run file d4grun,cmain32,int21dos option stack=32k,map export DOS4GOPTIONS=_DOS4GOPTIONS
   $(rsi_root)\bin\4gbind $(rsi_root)\bin\dos4g.exe w386run.exe d4grun.exe -V
dbgrun.exe : d4grun.obj cmain32.obj int21dos.obj
   wlink d a system dos4g name dbgrun file d4grun,cmain32,int21dos option stack=32k,map
tntrun.exe : tntrun.obj cmain32.obj int21pl.obj
   wlink system pharlap ext name tntrun file tntrun,cmain32,int21pl option stack=32k,map
   $(%tntdir)\bin\rebind $(%tntdir)\bin\rtk\tntb.exe tntrun.exp -exe tntrun.exe
x32run.exe : x32run.obj cmain32.obj x32start.obj
   *wlink format os2 lx name x32run file x32start,x32run,cmain32 lib x32b option map,dosseg,internalrelocs,stack=4k libpath $(lang_root)\lib386\dos;$(lang_root)\lib386
   x32fix x32run.exe
x32start.obj : x32start.asm
        masm /mx /s x32start;
tntrun.obj : loader.c loader.h
        wcc386 loader /ox /d1 /d__DOS /d__CALL21__ /d__TNT /fo=tntrun.obj /zq
x32run.obj : loader.c loader.h
        wcc386 loader /ox /d1 /d__DOS /d__CALL21__ /d__X32 /fo=x32run.obj /zq
d4grun.obj : loader.c loader.h dginfo.gh
        wcc386 loader /ox /d1 /d__DOS /d__CALL21__ /d__DOS4G /fo=d4grun.obj /zq
dginfo.gh : $(rsi_root)\bin\dos4g.exe
        @%write mkdginfo.bat $(rsi_root)\bin\dos4g $(rsi_root)\bin\dginfo >dginfo.gh
        -mkdginfo
int21dos.obj : int21dos.asm
        masm /mx /s int21dos;
int21pl.obj : int21pl.asm
        masm /mx /s int21pl;
cmain32.obj : cmain32.asm
        masm /mx /s cmain32;

dpmi.exe : dpmi.obj loader16.obj int21win.obj
        wlink system dos file dpmi,loader16,int21win option map,stack=8k
int21win.obj : int21win.asm
        masm /mx /s /Zi int21win;
dpmi.obj : dpmi.asm
        masm /mx /s /Zi dpmi;
dos16m.obj : dos16m.asm
        masm /mx /s /Zi dos16m;

clean: .SYMBOLIC
        @if exist *.exe @del *.exe
        @if exist *.exp @del *.exp
        @if exist *.obj @del *.obj
        @if exist *.map @del *.map
        @if exist *.gh @del *.gh
        @if exist mkdginfo.bat @del mkdginfo.bat
        @if exist a*. @del a*.
