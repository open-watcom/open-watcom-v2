
 Windows NT Video Miniport Programming Example
 ---------------------------------------------

 The sample video miniport supports the virtual SVGA chip supported by the
VirtualBox hypervisor. It may also work with bochs and qemu, but this has not
been tested; small changes may be required.

 The source code is split into two parts: The boxv module (boxv*) and the
miniport proper (videomp*). The boxv module provides a simple collection of
routines to set modes on the VirtualBox/qemu/bochs virtual SVGA chip. The
vidmini module implements the Windows NT video miniport interface.

 The miniport only provides minimal mode setting and palette access interface
since the virtual hardware does not support hardware cursors etc. All drawing
is left to framebuf.dll, the default Windows NT framebuffer display driver.

 Please refer to the Windows NT DDK for documentation on the video miniport
interface.


 Supported Windows Versions

 The miniport supports all Windows NT x86 versions from the original NT 3.1
up to 6.2 (Windows 7). Windows XP and above ships with a VGA/VESA driver which
provides similar functionality, but blocks power management (hibernation).


 Installation

 The installation method depends on Windows version used. For Windows NT 3.1,
the Setup applet must be used. For all other versions, the display property
dialog should be used. There are two versions of the INF file: oemsetup.inf
for Windows NT 3.x and vidmini.inf for Windows NT 4 and later versions.

 Important: For Windows NT 3.x, the framebuf.dll library provided on the NT
installation media is required and should be copied to the directory which
contains the INF file. For Windows NT 4.0 and later, framebuf.dll
is already preinstalled on the system.
