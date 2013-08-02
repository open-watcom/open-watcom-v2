#
# Linker initialization file - default systems
#
:segment dos
::
:: DOS, Windows (16-bit) default systems
::
# remember, don't put carets (^) in following two entries!
system begin 286
    libpath %WATCOM%\lib286
    libpath %WATCOM%\lib286\dos
    format dos
end
system begin 386
    option osname='DOS/4G'
    libpath %WATCOM%\lib386
    libpath %WATCOM%\lib386\dos
    op stub=wstub.exe
    format os2 le
end
:elsesegment nt
::
:: NT (16-bit) default system
::
# remember, don't put carets (^) in following two entries!
system begin 286
    option osname='Windows 16-bit'
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/win
    library windows
    option nocaseexact
    option stack=8k, heapsize=1k
    format windows
end
:segment axp
::
:: NT (AXP) default system
::
system begin 386
    option osname='Windows NT(AXP) character-mode'
    libpath %WATCOM%\libaxp
    libpath %WATCOM%\libaxp\nt
    format windows nt
    runtime console
end
:elsesegment
::
:: NT (x86) default system
::
system begin 386
    option osname='Windows NT character-mode'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt
    runtime console=4.0
end
:endsegment
:elsesegment linux
::
:: Linux (x86) default systems
::
# Note: Since there is no 16-bit target for Linux, we simply
# make the default 16-bit compiler produce 16-bit DOS apps.
#
# remember, don't put carets (^) in following two entries!
system begin 286
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/dos
    format dos
end
system begin 386
    option osname='Linux x86'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/linux
    format elf
    runtime linux
    op exportall
    op norelocs
end
:elsesegment os2
::
:: OS/2 default systems
::
# remember, don't put carets (^) in following two entries!
system begin 286
    option osname='OS/2 16-bit'
    libpath c:\os2
    libpath %WATCOM%\lib286
    libpath %WATCOM%\lib286\os2
    format os2
end
system begin 386
    option osname='OS/2 32-bit'
    libpath %WATCOM%\lib386
    libpath %WATCOM%\lib386\os2
    format os2 lx
end
:elsesegment qnx
::
:: QNX default systems
::
# remember, don't put carets (^) in following two entries!
system begin 286 
    option osname='QNX 16-bit'
    libpath %WATCOM%\lib286
    libpath %WATCOM%\lib286\qnx
    format qnx
end
system begin 386
    option osname='QNX 32-bit'
    libpath %WATCOM%\lib386
    libpath %WATCOM%\lib386\qnx
#    format qnx flat
    format qnx 
end
:elsesegment zdos
::
:: ZDOS default system
::
# remember, don't put carets (^) in following entry!
system begin 386
    option osname='ZDOS User Application'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/zdos
    libfile appstart.obj
    library clib3r.lib
    format zdos
end
:endsegment
