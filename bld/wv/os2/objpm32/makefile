#pmake: build os_os2 cpu_386
#
# WATCOM Debugger makefile for OS/2
#

# trmem = 1
host_OS=os2
host_CPU=386
display=gui
syshdir = $(mdir)\$(sysdir)\h;$(watcom_dir)\os220_h

#trap loader is different for 32-bit OS/2
dig_os_os2_386=os2v2

#add '32' suffix to avoid conflicts
tail_gui_os2=w32

sysobjs =   &
        os22ini.obj &
        os2chk.obj &
        os2cmd.obj &
        os22flio.obj &
        pmscrn2.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system os2v2_pm
    @%append $(lnk) imp WinThreadAssocQueue PMMERGE.5398
