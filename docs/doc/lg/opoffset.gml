.*
.*
.option OFFSET
.*
.np
For 32-bit RAW applications, the "OFFSET" option specifies the linear
base address of the raw output image.
.np
For OS/2, Win32 and ELF applications, the "OFFSET" option specifies the
preferred base linear address at which the executable or DLL will be
loaded.
.np
For 32-bit PharLap and QNX applications, the "OFFSET" option specifies
the offset in the program's segment in which the first byte of code or
data is loaded.
.*
.beglevel
.*
.section OFFSET - RAW only
.*
.np
The "OFFSET" option specifies the linear base address of the raw output image.
The format of the "OFFSET" option (short form "OFF") is as follows.
.mbigbox
    OPTION OFFSET=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the offset at which the output image will be located.
The following describes a use of the "OFFSET" option.
.np
.exam begin
option offset=0xc0000000
.exam end
.np
The image will be virtually/physically located to the linear address 0xc0000000.
.*
.section OFFSET - OS/2, Win32, ELF only
.*
.np
The "OFFSET" option specifies the preferred base linear address at
which the executable or DLL will be loaded.
The &lnkname will relocate the application for the specified base
linear address so that when it is loaded by the operating system, no
relocation will be required.
This decreases the load time of the application.
.np
If the operating system is unable to load the application at the
specified base linear address, it will load it at a different location
which will increase the load time since a relocation phase must be
performed.
.np
The format of the "OFFSET" option (short form "OFF") is as follows.
.mbigbox
    OPTION OFFSET=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
The "OFFSET" option is used to
specify the base linear address (in bytes) at which the program is
loaded and must be a multiple of 64K.
The linker will round the value up to a multiple of 64K if it is not
already a multiple of 64K.
The default base linear address is 64K for OS/2 executables and 4096K
for Win32 executables. For ELF, the default base address depends on the
CPU architecture.
.np
This option is most useful for improving the load time of DLLs,
especially for an application that uses multiple DLLs.
.*
.section OFFSET - PharLap only
.*
.np
The "OFFSET" option specifies the offset in the program's segment in
which the first byte of code or data is loaded.
The format of the "OFFSET" option (short form "OFF") is as follows.
.mbigbox
    OPTION OFFSET=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the offset (in bytes) at which the program is loaded and
must be a multiple of 4K.
The &lnkname will round the value up to a multiple of 4K if it is not
already a multiple of 4K.
.np
It is possible to detect NULL pointer references by linking the
program at an offset which is a multiple of 4K.
Usually an offset of 4K is sufficient.
.exam begin
option offset=4k
.exam end
.np
When the program is loaded by 386|DOS-Extender,
the pages skipped by the "OFFSET" option are not mapped.
Any reference to an unmapped area (such as a NULL pointer) will cause
a page fault preventing the NULL reference from corrupting the
program.
.*
.section OFFSET - QNX only
.*
.np
The "OFFSET" option specifies the offset in the program's segment in
which the first byte of code or data is loaded.
This option does not apply to 16-bit QNX applications.
The format of the "OFFSET" option (short form "OFF") is as follows.
.mbigbox
    OPTION OFFSET=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the offset (in bytes) at which the program is loaded and
must be a multiple of 4K.
The &lnkname will round the value up to a multiple of 4K if it is not
already a multiple of 4K.
The following describes a use of the "OFFSET" option.
.np
It is possible to detect NULL pointer references by linking the
program at an offset which is a multiple of 4K.
Usually an offset of 4K is sufficient.
.exam begin
option offset=4k
.exam end
.np
When the program is loaded,
the pages skipped by the "OFFSET" option are not mapped.
Any reference to an unmapped area (such as a NULL pointer) will cause
a page fault preventing the NULL reference from corrupting the
program.
.*
.endlevel
