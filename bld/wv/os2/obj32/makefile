#pmake: build os_os2 cpu_386
#
# WATCOM Debugger makefile for OS/2
#

host_OS=os2
host_CPU=386
display=char
extra_cflags= /zc
syshdir = $(mdir)\$(sysdir)\h;$(watcom_dir)\os220_h
#trap loader is different for 32-bit OS/2
dig_os_os2_386=os2v2

#add '32' suffix to avoid conflicts
tail_char_os2=32

sysobjs =   &
        os22ini.obj &
        os2chk.obj &
        os2cmd.obj &
        os22flio.obj &
        os2scrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system os2v2
    @%append $(lnk) option verbose
#    @%append $(lnk) option stack=32k
