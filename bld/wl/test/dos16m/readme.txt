
 DOS/16M notes
 =============

 There are two significantly different ways to create DOS/16M executables
(which usually use the .exp extension and a BW signature):

 1) MAKEPM - a normal DOS MZ executable is first created using a traditional
    DOS linker such as wlink or MS LINK. The MAKEPM tool (shipped with
    DOS/16M) is then used to convert the DOS EXE into a protected-mode
    executable.


 2) Directly - a linker which supports the DOS/16M executable format (GLU or
    wlink) is used to generate a protected-mode executable in a single step,
    without using the MAKEPM tool. The GLU tool is shipped with DOS/16M and
    DOS/4G.

 The first method (MAKEPM) is obsolete - DOS/4G only ships with GLU. There
are some fairly minor differences between the two methods.

 MAKEPM uses a special structure to collect certain information about the
executable and to enable relocations to DOS/16M kernel segments. See
d16_seginfo_struct in clib/startup/a/dos16m.asm. This structure is prefixed
by a "DOS/16M" string which MAKEPM looks for. The second method does not
need this structure, but it still needs help with creating the correct
segment relocations.

 Background note: DOS/16M significantly predates DPMI and old versions used
fixed GDT selector assignments. For example the executable's PSP would always
be accessible via selector 0x28 and the BIOS Data Area was accessible through
selector 0x40. With DPMI, these fixed assignments are no longer possible.
The executable must contain relocation records for the segment references and
the loader must fix the executable up.

 To create the appropriate segment relocations, the d16msels module (see
clib/startup/a/d16msels.asm) must be linked in. This module contains absolute
segment definitions for the fixed assignments. If DOS/16M is used on DOS with
no DPMI host, the segments are used directly and in fact no relocations are
needed at all. But in the typical case, the loader must fix up segment
references and remap the fixed assignments to the actual selectors used by
the host.

 DOS/4G and DOS/16M
 ------------------

 DOS/16M was the original DOS extender by Rational Systems, Inc. (RSI, now
Tenberry Software) developed in mid to late 1980s. At the beginning of the
1990s, RSI created DOS/4G. There are far more similarities between these two
products. DOS/4G is a superset of DOS/16M rather than an entirely different
DOS extender.

 In particular, DOS/4G supports running DOS/16M executables and also supports
the DOS/16M programming interface. The only caveat is that DOS/4GW (as opposed
to the full DOS/4G) refuses to load DOS/16M executables, unless they are
wsamprsi.exe or rsihelp.exp.

 However, because rsihelp.exp and wsamprsi.exe are actually 16-bit DOS/16M
executables, they support debugging/sampling of both 16-bit DOS/16M and
32-bit DOS/4G executables.
