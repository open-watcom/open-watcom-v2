.chap Missing Functions
.*
.np
The following is a list of functions that are not implemented on all
systems for which &company C/C++ is available.
Among the systems that are supported by &company C/C++ are:
.begnote $setptnt 12
.note DOS
This is DOS, a 16-bit real-mode system for Intel 80x86 computers.
.note DOS/32
This is DOS, extended for 32-bit protected-mode applications running
on Intel 386 or upward compatible systems.
.note Windows
This is Microsoft Windows 3.0, a 16-bit protect-mode system for Intel
286 and upwards compatible systems.
.note OS/2
This is IBM OS/2, a 16-bit protect-mode system for Intel 80286 and
upwards compatible systems.
When "(all)" appears after OS/2, it means all versions of the OS/2
libraries.
The "OS/2-mt" refers to the OS/2
.filename CLIBMTL
library which supports multi-threaded applications.
The "OS/2-dll" refers to the OS/2
.filename CLIBDLL
library which supports creation of Dynamic Link Libraries.
If a function is missing from the OS/2 library, it may be found in
&company's 16-bit protect-mode DOS libraries for the PharLap 286 DOS
Extender (DOSPM*.LIB).
.endnote
.np
The first column contains the name of the function or variable which
is not supported by the system(s) named in the second column.
Variables are marked "(var)".
.begnote $compact $setptnt 25
.termhd1 Function / Variable
.termhd2 Unsupported System
.note bdos
OS/2 (all)
.note _beginthread
DOS, DOS/32, Windows
.note _bios_disk
OS/2 (all)
.note _bios_equiplist
OS/2 (all)
.note _bios_keybrd
OS/2 (all)
.note _bios_memsize
OS/2 (all)
.note _bios_printer
OS/2 (all)
.note _bios_serialcom
OS/2 (all)
.note _bios_timeofday
OS/2 (all)
.note _chain_intr
OS/2 (all)
.note cwait
DOS, DOS/32, Windows
.note _dos_allocmem
Windows, OS/2 (all)
.note _dos_close
OS/2 (all)
.note _dos_creat
OS/2 (all)
.note _dos_creatnew
OS/2 (all)
.note _dos_findfirst
OS/2 (all)
.note _dos_findnext
OS/2 (all)
.note _dos_freemem
Windows, OS/2 (all)
.note _dos_getdate
OS/2 (all)
.note _dos_getdiskfree
OS/2 (all)
.note _dos_getdrive
OS/2 (all)
.note _dos_getfileattr
OS/2 (all)
.note _dos_getftime
OS/2 (all)
.note _dos_gettime
OS/2 (all)
.note _dos_getvect
OS/2 (all)
.note _dos_keep
Windows, OS/2 (all)
.note _dos_open
OS/2 (all)
.note _dos_read
OS/2 (all)
.note _dos_setblock
Windows, OS/2 (all)
.note _dos_setdate
OS/2 (all)
.note _dos_setdrive
OS/2 (all)
.note _dos_setfileattr
OS/2 (all)
.note _dos_setftime
OS/2 (all)
.note _dos_settime
OS/2 (all)
.note _dos_setvect
OS/2 (all)
.note _dos_write
OS/2 (all)
.note _doserrno (var)
OS/2-mt, OS/2-dll
.note dosexterr
OS/2 (all)
.note _endthread
DOS, DOS/32, Windows
.note execl
DOS/32, Windows
.note execle
DOS/32, Windows
.note execlp
DOS/32, Windows
.note execlpe
DOS/32, Windows
.note execv
DOS/32, Windows
.note execve
DOS/32, Windows
.note execvp
DOS/32, Windows
.note execvpe
DOS/32, Windows
.note _fexpand
DOS/32
.note _ffree
DOS/32
.note _fheapchk
DOS/32
.note _fheapgrow
DOS/32
.note _fheapmin
DOS/32
.note _fheapset
DOS/32
.note _fheapshrink
DOS/32
.note _fheapwalk
DOS/32
.note _fmalloc
DOS/32
.note _fmsize
DOS/32
.note _frealloc
DOS/32
.note halloc
DOS/32
.note _harderr
DOS/32, Windows, OS/2 (all)
.note _hardresume
DOS/32, Windows, OS/2 (all)
.note _hardretn
DOS/32, Windows, OS/2 (all)
.note _heapgrow
DOS/32
.note hfree
DOS/32
.note int386
DOS, Windows, OS/2 (all)
.note int386x
DOS, Windows, OS/2 (all)
.note int86
DOS/32, OS/2 (all)
.note int86x
DOS/32, OS/2 (all)
.note intdos
OS/2 (all)
.note intdosx
OS/2 (all)
.note intr
OS/2 (all)
.note _nfree
OS/2-dll
.note _nheapchk
OS/2-dll
.note _nheapgrow
DOS/32, OS/2-dll
.note _nheapset
OS/2-dll
.note _nheapshrink
OS/2-dll
.note _nheapwalk
OS/2-dll
.note _nmalloc
OS/2-dll
.note nosound
OS/2 (all)
.note _nrealloc
OS/2-dll
.note _osmode (var)
DOS/32, Windows
.note pAtomTable (var)
DOS, DOS/32, OS/2 (all)
.note pLocalHeap (var)
DOS, DOS/32, OS/2 (all)
.note _psp (var)
OS/2 (all)
.note pStackBot (var)
DOS, DOS/32, OS/2 (all)
.note pStackMin (var)
DOS, DOS/32, OS/2 (all)
.note pStackTop (var)
DOS, DOS/32, OS/2 (all)
.note sbrk
OS/2-dll
.note sound
OS/2 (all)
.note spawnl
Windows
.note spawnle
Windows
.note spawnlp
Windows
.note spawnlpe
Windows
.note spawnv
Windows
.note spawnve
Windows
.note spawnvp
Windows
.note spawnvpe
Windows
.note system
Windows
.note _threadid (var)
DOS, DOS/32, Windows, OS/2
.note wait
DOS, DOS/32, Windows
.endnote
