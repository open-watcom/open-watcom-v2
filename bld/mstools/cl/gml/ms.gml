:cmt **********************************************************************
:cmt *       Copyright by WATCOM International Corporation, 1987, 1992.   *
:cmt *       All rights reserved. No part of this software may be         *
:cmt *       reproduced in any form or by any means - graphic, electronic,*
:cmt *       mechanical or otherwise, including, without limitation,      *
:cmt *       photocopying, recording, taping or information storage and   *
:cmt *       retrieval systems - except with the written permission of    *
:cmt *       WATCOM International Corporation.                            *
:cmt **********************************************************************
:cmt
:cmt    Modified    By              Reason
:cmt    --------    --              ------
:cmt    21-nov-95   M. Hildebrand   Initial implementation.
:cmt    14-feb-96   T. Schiller     Added /nowopts and /passwopts options.
:cmt    27-feb-96   Greg Bentz      Added /nowwarn option.
:cmt    29-feb-96   A. Kucharczyk   Added /lesswd option.
:cmt	16-may-96   Greg Bentz	    Chain F, G and O options.
:cmt	10-jul-96   Greg Bentz	    Added /noinvoke option.
:cmt
:cmt GML Macros used:
:cmt
:cmt    :chain. <char> <usage>                  options that start with <char>
:cmt                                            can be chained together e.g.,
:cmt                                            -oa -ox -ot => -oaxt
:cmt    :option. <option> <synonym> ...         define an option
:cmt    :target. <arch1> <arch2> ...            valid for these architectures
:cmt    :ntarget. <arch1> <arch2> ...           not valid for these architectures
:cmt    :immediate. <fn>                        <fn> is called when option parsed
:cmt    :enumerate. <field> [<value>]           option is one value in <name> enumeration
:cmt    :number. [<fn>] [<default>]             =<n> allowed; call <fn> to check
:cmt    :id. [<fn>]                             =<id> req'd; call <fn> to check
:cmt    :char.[<fn>]                            =<char> req'd; call <fn> to check
:cmt    :file.                                  =<file> req'd
:cmt    :path.                                  =<path> req'd
:cmt    :special. <fn>                          call <fn> to parse option
:cmt    :optional.                              value is optional
:cmt    :internal.                              option is undocumented
:cmt    :prefix.                                prefix of a :special. option
:cmt    :usage. <text>                          English usage text
:cmt    :jusage. <text>                         Japanese usage text
:cmt    :title.                                 English usage text
:cmt    :timestamp.                             kludge to record "when" an option
:cmt                                            is set so that dependencies
:cmt                                            between options can be simulated
:cmt    :noequal.                               usage message contains /op<value>,
                                                not /op=<value>

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx

:title. Usage: cl [options] file [options]
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any

:noequal.

:chain. F file options
:chain. G code generation options
:chain. O optimization options
:chain. Z language options

:option. 10x_
:target. any
:internal.
:usage. use 10.x options

:option. C
:target. any
:usage. preserve comments

:option. c
:target. any
:usage. compile only

:option. D
:target. any
:special. parse_D <macro>[=<value>]
:usage. same as #define <macro>[=<value>] before compilation

:option. E
:target. any
:usage. preprocess and insert #line directives to stdout

:option. EP
:target. any
:usage. preprocess without #line directives to stdout

:option. F
:target. any
:special. parse_F <size>
:immediate. handle_F
:usage. set stack size

:option. Fa
:target. any
:path.
:internal.
:usage. specify listing output file

:option. FA
:target. any
:internal.
:usage. generate assembly listing

:option. FAc
:target. any
:internal.
:usage. generate assembly and machine code listing

:option. FAcs
:target. any
:internal.
:usage. generate assembly, source, and machine code listing

:option. FAs
:target. any
:internal.
:usage. generate assembly and source listing

:option. Fd
:target. any
:file.
:internal.
:usage. specify PDB filename

:option. Fe
:target. any
:immediate. handle_Fe
:file.
:usage. set executable or DLL file name

:option. FI
:target. any
:special. parse_FI <file>
:usage. force <file> to be included

:option. Fm
:target. any
:special. parse_Fm [<file>]
:usage. set map file name

:option. Fo
:target. any
:file.
:usage. set object output file name

:option. Fp
:target. any
:immediate. handle_Fp
:file.
:usage. set precompiled header data file name

:option. FR
:target. any
:immediate. handle_FR
:file.
:usage. generate browsing information

:option. Fr
:target. any
:internal.
:usage. generate SBR file without local variables

:option. G3
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions

:option. G4
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486

:option. G5
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium

:option. GB
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486

:option. Gd
:target. any
:enumerate. calling_convention
:internal.
:usage. use __cdecl (stack-based) calling convention

:option. Ge
:target. any
:enumerate. stack_probes
:immediate. handle_stack_probes
:usage. activate stack probes for all functions

:option. Gf
:target. any
:usage. merge duplicate strings

:option. GF
:target. any
:usage. merge duplicate read-only strings

:option. Gh
:target. any
:usage. call __penter at the start of each function

:option. Gr
:target. any
:enumerate. calling_convention
:internal.
:usage. use __fastcall (register-based) calling convention

:option. Gs
:target. any
:enumerate. stack_probes
:immediate. handle_stack_probes
:special. parse_Gs <distance>
:usage. set stack probe distance

:option. GX
:target. any
:immediate. handle_GX
:usage. destruct static objects during stack unwinding

:option. Gy
:target. any
:usage. store each function in its own COMDAT

:option. Gz
:target. any
:enumerate. calling_convention
:internal.
:usage. use __stdcall (register-based) calling convention

:option. H
:target. any
:number.
:internal.
:usage. set maximum identifier length

:option. help HELP ?
:target. any
:usage. get help

:option. I
:target. any
:special. parse_I <path>
:usage. add another include path

:option. J
:target. any
:usage. change char default from signed to unsigned

:option. link
:target. any
:special. parse_link
:usage. specify linker options

:option. LD
:target. any
:usage. create DLL

:option. MD
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded DLL version of C library

:option. MDd
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded debug DLL version of C library

:option. ML
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use single-thread statically linked version of C library

:option. MLd
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use single-thread debug static link version of C library

:option. MT
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded static version of C library

:option. MTd
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded debug static version of C library

:option. nologo
:target. any
:usage. operate quietly

:option. O1
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. minimize size

:option. O2
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. maximize speed

:option. Oa
:target. any
:usage. assume no aliasing

:option. Ob
:target. any
:immediate. handle_inlining_level
:number. check_inlining_level
:usage. control function inlining

:option. Od
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. disable all optimizations

:option. Og
:target. any
:usage. enable global optimizations

:option. Oi
:target. any
:usage. expand intrinsic functions inline

:option. Op
:target. any
:immediate. handle_Op
:timestamp.
:usage. generate consistent floating-point results

:option. Os
:target. any
:immediate. handle_opt_size_time
:enumerate. opt_size_time
:usage. favor code size over execution time in optimizations

:option. Ot
:target. any
:immediate. handle_opt_size_time
:enumerate. opt_size_time
:usage. favor execution time over code size in optimizations

:option. Ow
:target. any
:internal.
:usage. assume aliasing across function calls

:option. Ox
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. equivalent to /Ob1 /Og /Oi /Ot /Oy /Gs

:option. Oy
:target. any
:immediate. handle_Oy
:usage. disable stack frames

:option. P
:target. any
:usage. preprocess to a file

:option. showwopts
:target. any
:usage. show translated options

:option. passwopts
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:option. nowopts
:target. any
:usage. disable default options

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. lesswd
:target. any
:internal.
:usage. change debug info from -d2 to -d1

:option. Tc
:target. any
:special. parse_Tc <file>
:usage. force compilation of <file> as C

:option. Tp
:target. any
:special. parse_Tp <file>
:usage. force compilation of <file> as C++

:option. U
:target. any
:special. parse_U <macro>
:usage. undefine macro name

:option. u
:target. any
:internal.
:usage. undefine all predefined macros

:option. vd0
:target. any
:internal.
:usage. disable constructor/destructor displacements

:option. vd1
:target. any
:internal.
:usage. enable vtordisp constructor/destructor displacements

:option. vmb
:target. any
:internal.
:usage. use best case always pointer representation

:option. vmg
:target. any
:internal.
:usage. use general purpose always pointer representation

:option. vmm
:target. any
:internal.
:usage. general-purpose pointers to single- and multiple-inheritance classes

:option. vms
:target. any
:internal.
:usage. general-purpose pointers to single-inheritance classes

:option. vmv
:target. any
:internal.
:usage. general-purpose pointers to any classes

:option. V
:target. any
:special. parse_V
:internal.
:usage. embed string in object file

:option. W
:target. any
:immediate. handle_warn_level
:enumerate. warn_level
:number. check_warn_level
:usage. set warning level number

:option. w
:target. any
:immediate. handle_warn_level
:enumerate. warn_level
:usage. disable all warning messages

:option. WX
:target. any
:usage. treat all warnings as errors

:option. X
:target. any
:internal.
:usage. ignore standard include paths

:option. Yc
:target. any
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. create pre-compiled header file

:option. Yd
:target. any
:usage. full debug info from pre-compiled headers

:option. Yu
:target. any
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. use pre-compiled header file

:option. YX
:target. any
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. use pre-compiled header file

:option. Z7
:target. any
:immediate. handle_debug_info
:enumerate. debug_info
:usage. generate Codeview debugging information

:option. Za
:target. any
:enumerate. iso
:timestamp.
:usage. disable extensions (i.e., accept only ISO/ANSI C++)

:option. Zd
:target. any
:immediate. handle_debug_info
:enumerate. debug_info
:usage. line number debugging information

:option. Ze
:target. any
:enumerate. iso
:usage. enable extensions (e.g., near, far, export, etc.)

:option. Zg
:target. any
:usage. output function declarations to stdout

:option. Zi
:target. any
:immediate. handle_debug_info
:enumerate. debug_info
:internal.
:usage. full symbolic debugging information

:option. Zl
:target. any
:usage. remove default library information

:option. Zn
:target. any
:internal.
:usage. disable SBR file packing

:option. Zp
:target. any
:number. check_packing 1
:usage. pack structure members with alignment {1,2,4,8,16}

:option. Zs
:target. any
:usage. syntax check only
