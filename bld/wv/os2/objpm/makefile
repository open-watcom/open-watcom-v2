#pmake: build os_os2 cpu_i86 i86 intel
#
# WATCOM Debugger makefile for OS/2
#

# trmem = 1
host_OS=os2
host_CPU=i86
display=gui
extra_cflags= /zc /zu
syshdir = $(mdir)\$(sysdir)\h;$(watcom_dir)\os21x_h

sysobjs =   &
        os2init.obj &
        os2lkup.obj &
        os2chk.obj &
        os2cmd.obj &
        os2filio.obj &
        pmscrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system os2 pm
    @%append $(lnk) option newfile
    @%append $(lnk) option stack=20k
    @%append $(lnk) option map
