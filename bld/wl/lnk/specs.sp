::
:: this is a splice that will expand into either wlsystem.lnk
:: and the new, to-be-written, specs.owc control file for owcc
::
:: for the moment: just a copy of the original wlsystem.lnk:
#
:segment Pwlsystem
# Linker initialization file - common systems
:elsesegment Pspecs
# default specs.owc file
:: FIXME: should -bd and others also be passed?
:endsegment
#
system begin dos
:segment Pspecs
    ARCH i86 -bt=dos
:elsesegment Pwlsystem
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/dos
    format dos ^
:endsegment
end
system begin dos4g
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/4G'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    op stub=wstub.exe
    format os2 le
:endsegment
end
system begin dos4gnz
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/4G non-zero base'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    disable 123     # non-64k offset option OK here.
    op internalrelocs
    op togglerelocs
    op stub=wstub.exe
    format os2 lx
:endsegment
end
system begin pharlap
:segment Pspecs
    ARCH i386 -bt=dos
:: FIXME: need -ez to generate Easy OMF-386?
:elsesegment Pwlsystem
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    format phar ^
:endsegment
end
system begin x32r
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='FlashTek (register calling convention)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile cstrtx3r.obj
    option stack=4k,align=4k,internalrelocs
    library x32b.lib
    format os2 lx ^
:endsegment
end
system begin x32rv
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='FlashTek Virtual Memory (register calling convention)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile cstrtx3r.obj
    option stack=4k,align=4k,internalrelocs
    library x32vb.lib
    format os2 lx ^
:endsegment
end
system begin x32s
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='FlashTek (stack calling convention)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile cstrtx3s.obj
    option stack=4k,align=4k,internalrelocs
    library x32b.lib
    format os2 lx ^
:endsegment
end
system begin x32sv
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='FlashTek Virtual Memory (stack calling convention)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile cstrtx3s.obj
    option stack=4k,align=4k,internalrelocs
    library x32vb.lib
    format os2 lx ^
:endsegment
end
system begin win386
:segment Pspecs
    ARCH i386 -bt=windows
:elsesegment Pwlsystem
    option osname='Windows 32-bit'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/win
    format phar rex
:endsegment
end
system begin os2
:segment Pspecs
    ARCH i86 -bt=os2
:elsesegment Pwlsystem
    option osname='OS/2 16-bit'
    library os2.lib
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/os2
    format os2 ^
:endsegment
end
system begin os2_pm
:segment Pspecs
    ARCH i86 -bt=os2
:elsesegment Pwlsystem
    option osname='OS/2 16-bit Presentation Manager'
    library os2.lib
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/os2
    format os2 pm ^
:endsegment
end
system begin os2_dll
:segment Pspecs
    ARCH i86 -bt=os2 -bd
:elsesegment Pwlsystem
    option osname='OS/2 16-bit'
    library os2.lib
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/os2
    format os2 dll ^
:endsegment
end
system begin windows
:segment Pspecs
    ARCH i86 -bt=windows
:elsesegment Pwlsystem
    option osname='Windows 16-bit'
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/win
    library windows
    option nocaseexact
    option stack=8k, heapsize=1k
    format windows ^
:endsegment
end
system begin windows_dll
:segment Pspecs
    ARCH i86 -bt=windows -bd
:: FIXME: need bd?
:elsesegment Pwlsystem
    option osname='Windows 16-bit'
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/win
    library windows
    option nocaseexact
    option stack=8k, heapsize=1k
    libfile libentry.obj
    format windows dll ^
:endsegment
end
system begin win_vxd
:segment Pspecs
    ARCH i386
:: FIXME: which bt=?
:elsesegment Pwlsystem
    option osname='Windows VxD'
    format windows vxd ^
:endsegment
end
system begin os2v2
:segment Pspecs
    ARCH i386 -bt=os2
:elsesegment Pwlsystem
    option osname='OS/2 32-bit'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/os2
    format os2 lx ^
:endsegment
end
system begin os2v2_pm
:segment Pspecs
    ARCH i386 -bt=os2
:elsesegment Pwlsystem
    option osname='OS/2 32-bit Presentation Manager'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/os2
    format os2 lx pm ^
:endsegment
end
system begin os2v2_dll
:segment Pspecs
    ARCH i386 -bt=os2 -bd
:elsesegment Pwlsystem
    option osname='OS/2 32-bit'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/os2
    format os2 lx dll ^
:endsegment
end
system begin novell
:segment Pspecs
    ARCH i386 -bt=netware
:elsesegment Pwlsystem
    option osname='Novell NLM'
    format novell ^
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/netware
    module clib
    import @%WATCOM%/novi/clib.imp
:endsegment
end
system begin netware
:segment Pspecs
    ARCH i386 -bt=netware
:elsesegment Pwlsystem
    option osname='Novell NLM'
    format novell ^
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/netware
    module clib
    import @%WATCOM%/novi/clib.imp
:endsegment
end
system begin netware_libc
:segment Pspecs
    ARCH i386 -bt=netware
:elsesegment Pwlsystem
    option osname='NetWare LibC NLM (Full-RTL)'
    format novell ^
    option nodefault
    option start=_LibCPrelude
    option exit=_LibCPostlude
    option pseudopreemption
    import CurrentProcess
    file %NOVELLNDK%/../libc/imports/libcpre.obj
    library %NOVELLNDK%/../libc/imports/libcaux.wcc
    library %WATCOM%/lib386/netware/libc3s.lib
    import @%NOVELLNDK%/../libc/imports/libc.imp
    import @%NOVELLNDK%/../libc/imports/netware.imp
:endsegment
end
system begin netware_libc_lite
:segment Pspecs
    ARCH i386 -bt=netware
:elsesegment Pwlsystem
    option osname='NetWare LibC NLM (Lite-RTL)'
    format novell ^
    option nodefault
    option start=_LibCPrelude
    option exit=_LibCPostlude
    option pseudopreemption
    import CurrentProcess
    file %NOVELLNDK%/../libc/imports/libcpre.obj
    library %NOVELLNDK%/../libc/imports/libcaux.wcc
    library %WATCOM%/lib386/netware/libc3sl.lib
    import @%NOVELLNDK%/../libc/imports/libc.imp
    import @%NOVELLNDK%/../libc/imports/netware.imp
:endsegment
end
system begin netware_clib
:segment Pspecs
    ARCH i386 -bt=netware
:elsesegment Pwlsystem
    option osname='NetWare CLIB NLM (Full-RTL)'
    format novell ^
    option nodefault
    option start=_Prelude
    option exit=_Stop
    library %WATCOM%/lib386/netware/clib3s.lib
    import @%NOVELLNDK%/imports/clib.imp
:endsegment
end
system begin netware_clib_lite
:segment Pspecs
    ARCH i386 -bt=netware
:elsesegment Pwlsystem
    option osname='NetWare CLIB NLM (Lite-RTL)'
    format novell ^
    option nodefault
    option start=_Prelude
    option exit=_Stop
    file %NOVELLNDK%/imports/clibpre.obj
    library %WATCOM%/lib386/netware/clib3sl.lib
    import @%NOVELLNDK%/imports/clib.imp
:endsegment
end
system begin ads
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='AutoCAD Development System'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile adsstart.obj
    format phar ext ^
:endsegment
end
system begin eadi
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='emulation AutoCAD Device Interface'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile adiestrt.obj
    format phar ext ^
:endsegment
end
system begin fadi
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='floating point AutoCAD Device Interface'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libfile adifstrt.obj
    format phar ext ^
:endsegment
end
system begin com
:segment Pspecs
    ARCH i86 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS .COM'
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/dos
    libfile cstart_t.obj
    format dos com
:endsegment
end
system begin qnx
:segment Pspecs
    ARCH i86 -bt=qnx
:elsesegment Pwlsystem
    option osname='QNX 16-bit'
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/qnx
    format qnx
:endsegment
end
system begin qnx386
:segment Pspecs
    ARCH i386 -bt=qnx
:elsesegment Pwlsystem
    option osname='QNX 32-bit'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/qnx
    format qnx ^
:endsegment
end
system begin linux
:segment Pspecs
    ARCH i386 -bt=linux
:elsesegment Pwlsystem
    option osname='Linux x86'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/linux
    format elf
    runtime linux
    op exportall
    op norelocs
:endsegment
end
system begin linuxmips
:segment Pspecs
    ARCH mips -bt=linux
:elsesegment Pwlsystem
    option osname='Linux MIPS'
    libpath %WATCOM%/libmps
    libpath %WATCOM%/libmps/linux
    format elf
    runtime linux
    op exportall
    op norelocs
:endsegment
end
system begin nt
:segment Pspecs
    ARCH i386 -bt=nt
:elsesegment Pwlsystem
    option osname='Windows NT character-mode'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    libpath %WATCOM%/lib386/nt/directx
    libpath %WATCOM%/lib386/nt/ddk
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt ^
    runtime console=4.0
:endsegment
end
system begin nt_win
:segment Pspecs
    ARCH i386 -bt=nt
:elsesegment Pwlsystem
    option osname='Windows NT windowed'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    libpath %WATCOM%/lib386/nt/directx
    libpath %WATCOM%/lib386/nt/ddk
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt ^
    runtime windows=4.0
:endsegment
end
system begin nt_dll
:segment Pspecs
    ARCH i386 -bt=nt -bd
:elsesegment Pwlsystem
    option osname='Windows NT'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    libpath %WATCOM%/lib386/nt/directx
    libpath %WATCOM%/lib386/nt/ddk
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt dll ^
    runtime windows=4.0
:endsegment
end
system begin win95
:segment Pspecs
    ARCH i386 -bt=nt
:elsesegment Pwlsystem
    option osname='Windows 95'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    libpath %WATCOM%/lib386/nt/directx
    libpath %WATCOM%/lib386/nt/ddk
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt ^
    runtime windows=4.0
:endsegment
end
system begin win32
:segment Pspecs
    ARCH i386 -bt=nt
:elsesegment Pwlsystem
    option osname='Win32'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    libpath %WATCOM%/lib386/nt/directx
    libpath %WATCOM%/lib386/nt/ddk
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt ^
    runtime windows=4.0
:endsegment
end
system begin win32s
:segment Pspecs
    ARCH i386 -bt=nt
:elsesegment Pwlsystem
    option osname='Win32s (Windows 3.x)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/nt
    library kernel32,user32,gdi32,advapi32,comdlg32,winspool,shell32,version,ole32
    library netapi32,dlcapi,mpr,rpcns4,rpcrt4,vdmdbg,winmm,winstrm,wsock32,lz32,win32spl
    format windows nt ^
    runtime windows
:endsegment
end
system begin tnt
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='Phar Lap TNT DOS style'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    format windows nt tnt ^
    runtime dosstyle
:endsegment
end
system begin ntaxp
:segment Pspecs
    ARCH axp -bt=nt
:elsesegment Pwlsystem
    option osname='Windows NT(AXP) character-mode'
    libpath %WATCOM%/libaxp
    libpath %WATCOM%/libaxp/nt
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt ^
    runtime console=4.0
:endsegment
end
system begin ntaxp_win
:segment Pspecs
    ARCH axp -bt=nt
:elsesegment Pwlsystem
    option osname='Windows NT(AXP) windowed'
    libpath %WATCOM%/libaxp
    libpath %WATCOM%/libaxp/nt
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt ^
    runtime windows=4.0
:endsegment
end
system begin ntaxp_dll
:segment Pspecs
    ARCH axp -bt=nt -bd
:elsesegment Pwlsystem
    option osname='Windows NT(AXP)'
    libpath %WATCOM%/libaxp
    libpath %WATCOM%/libaxp/nt
    library kernel32,user32,gdi32,advapi32,comdlg32,ole32,oleaut32,winspool,shell32,uuid,comctl32
    format windows nt dll ^
    runtime windows=4.0
:endsegment
end
system begin causeway
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='CauseWay'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    op stub=cwstub.exe
    format os2 le ^
:endsegment
end
system begin cwdlls
:segment Pspecs
    ARCH i386 -bt=dos
:: FIXME: -bd??
:elsesegment Pwlsystem
    option osname='CauseWay (stack calling convention)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    format os2 le dll ^
    libfile dllstrts.obj
:endsegment
end
system begin cwdllr
:segment Pspecs
    ARCH i386 -bt=dos
:: FIXME: -bd??
:elsesegment Pwlsystem
    option osname='CauseWay (register calling convention)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    format os2 le dll ^
    libfile dllstrtr.obj
:endsegment
end
system begin dos32a
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/32 Advanced DOS Extender (LE-style)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libpath %WATCOM%/lib386/l32
    op stub=dos32a.exe
    format os2 le
:endsegment
end
system begin dos32x
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/32 Advanced DOS Extender (LX-style)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libpath %WATCOM%/lib386/l32
#   debug watcom all
#   op symfile
    op stub=dos32a.exe
    op internalrelocs
    op align=16
    format os2 lx
:endsegment
end
system begin stub32a
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/32A DOS Extender w/ Standard stub (LE-style)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libpath %WATCOM%/lib386/l32
    op stub=stub32a.exe
    format os2 le
:endsegment
end
system begin stub32x
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/32A DOS Extender w/ Standard stub (LX-style)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libpath %WATCOM%/lib386/l32
#   debug watcom all
#   op symfile
    op stub=stub32a.exe
    op internalrelocs
    op align=16
    format os2 lx
:endsegment
end
system begin stub32ac
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/32A DOS Extender w/ Configurable stub (LE-style)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libpath %WATCOM%/lib386/l32
    op stub=stub32c.exe
    format os2 le
:endsegment
end
system begin stub32xc
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='DOS/32A DOS Extender w/ Configurable stub (LX-style)'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    libpath %WATCOM%/lib386/l32
#   debug watcom all
#   op symfile
    op stub=stub32c.exe
    op internalrelocs
    op align=16
    format os2 lx
:endsegment
end
system begin pmodew
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='PMODE/W'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    op stub=pmodew.exe
    format os2 le
:endsegment
end
system begin pmodewi
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='PMODE/WI'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    op stub=pmodewi.exe
    format os2 le
:endsegment
end
system begin wdosx
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='WDOSX'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    op stub=wdosxle.exe
    format os2 le
:endsegment
end
system begin zrdx
:segment Pspecs
    ARCH i386 -bt=dos
:elsesegment Pwlsystem
    option osname='Zurenava'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/dos
    op stub= zrdx.exe
    format os2 le
:endsegment
end
system begin dos16m
:segment Pspecs
    ARCH i86 -bt=dos
:elsesegment Pwlsystem
    libpath %WATCOM%/lib286
    libpath %WATCOM%/lib286/dos
    libfile dos16m.obj
    libfile d16msels.obj
    format dos16m runtime auto ^
:endsegment
end
system begin zdos
:segment Pspecs
    ARCH i386 -bt=zdos
:elsesegment Pwlsystem
    option osname='ZDOS User Application'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/zdos
    libfile appstart.obj
    format zdos
:endsegment
end
system begin zdosfsd
:segment Pspecs
    ARCH i386 -bt=zdos
:elsesegment Pwlsystem
    option osname='ZDOS File System Driver'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/zdosdrv
    libfile fsdstart.obj
    format zdos fsd
:endsegment
end
system begin zdoshwd
:segment Pspecs
    ARCH i386 -bt=zdos
:elsesegment Pwlsystem
    option osname='ZDOS Hardware Driver'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/zdosdrv
    libfile hwdstart.obj
    format zdos hwd
:endsegment
end
system begin zdosdev
:segment Pspecs
    ARCH i386 -bt=zdos
:elsesegment Pwlsystem
    option osname='ZDOS Device Driver'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/zdosdrv
    libfile devstart.obj
    format zdos sys
:endsegment
end
system begin rdos
:segment Pspecs
    ARCH i386 -bt=rdos
:elsesegment Pwlsystem
    option osname='RDOS'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/rdos
    format windows pe rdos ^
:endsegment
end
system begin rdos_dll
:segment Pspecs
    ARCH i386 -bt=rdos -bd
:elsesegment Pwlsystem
    option osname='RDOS'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/rdos
    format windows pe rdos dll ^
:endsegment
end
system begin rdos_efi
:segment Pspecs
    ARCH i386 -bt=rdos -bd
:elsesegment Pwlsystem
    option osname='RDOS'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/rdos
    format windows pe efi ^
:endsegment
end
system begin rdos_dev32
:segment Pspecs
    ARCH i386 -bt=rdosdev -zu
:elsesegment Pwlsystem
    option osname='32-bit RDOS Device driver'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/rdosdev
    format rdos dev32 ^
:endsegment
end
system begin rdos_dev16
:segment Pspecs
    ARCH i86 -bt=rdos_dev16
:elsesegment Pwlsystem
    option osname='16-bit RDOS Device driver'
    format rdos dev16 ^
:endsegment
end
system begin rdosdev
:segment Pspecs
    ARCH i386 -bt=rdosdev -zu
:elsesegment Pwlsystem
    option osname='32-bit RDOS Device driver'
    libpath %WATCOM%/lib386
    libpath %WATCOM%/lib386/rdosdev
    format rdos dev32 ^
:endsegment
end
system begin rdos_bin32
:segment Pspecs
    ARCH i386 -bt=rdos_bin32
:elsesegment Pwlsystem
    option osname='32-bit RDOS Binary'
    libpath %WATCOM%/lib386
    format rdos bin32 ^
:endsegment
end
system begin rdos_bin16
:segment Pspecs
    ARCH i86 -bt=rdos_bin16
:elsesegment Pwlsystem
    option osname='16-bit RDOS Binary'
    format rdos bin16 ^
:endsegment
end
system begin rdos_mboot
:segment Pspecs
    ARCH i86 -bt=rdos_mboot
:elsesegment Pwlsystem
    option osname='RDOS Multiboot Stub'
    format rdos mboot ^
:endsegment
end
:segment Pwlsystem
system begin rawbin
    option osname='RAW Binary Image'
    format raw bin
end
system begin rawhex
    option osname='RAW Intel Hex Image'
    format raw hex
end
:endsegment
