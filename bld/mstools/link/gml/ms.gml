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
:cmt    05-feb-96   T. Schiller     Modified BASE option.
:cmt	14-feb-96   T. Schiller	    Added /nowopts and /passwopts options.
:cmt	27-feb-96   Greg Bentz	    Added /nowwarn option.
:cmt	07-jun-96   M. Hildebrand   Added /nofuzzy option.
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

:title. Usage: link [options] file [options]
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any

:argequal. :


:option. 10x_
:target. any
:internal.
:usage. use 10.x options

:option. align
:target. any
:number. check_align
:usage. specify alignment of each section

:option. base
:target. any
:special. parse_base :<arg>
:usage. set program base address

:option. comment
:target. any
:special. parse_comment :<string>
:usage. embed comment string into image

:option. debug
:target. any
:special. parse_debug
:immediate. handle_debug
:usage. link in debugging information

:option. debugtype
:target. any
:special. parse_debugtype
:internal.
:usage. link specified type of debugging information

:option. def
:target. any
:special. parse_def :<file>
:usage. process .def file

:option. defaultlib
:target. any
:special. parse_defaultlib
:usage. add one or more libraries to search

:option. dll
:target. any
:usage. build DLL

:option. entry
:target. any
:special. parse_entry :<symbol>
:usage. set starting address

:option. exetype
:target. any
:special. parse_exetype
:internal.
:usage. specify type of VXD to build

:option. export
:target. any
:special. parse_export :<exportdef>
:usage. export a symbol

:option. fixed
:target. any
:internal.
:usage. prevent program relocation when loading

:option. force:multiple
:target. any
:usage. allow multiply defined symbols

:option. force:undefined
:target. any
:usage. allow undefined symbols

:option. heap
:target. any
:special. parse_heap :<reserve>[,<commit>]
:usage. set heap size in bytes

:option. implib
:target. any
:special. parse_implib :<file>
:usage. override default import library name

:option. include
:target. any
:special. parse_include :<symbol>
:usage. force reference to a symbol

:option. incremental
:target. any
:special. parse_incremental :yes|no
:usage. enable or disable incremental linking

:option. internaldllname_
:target. any
:special. parse_internaldllname_
:internal.
:usage. enable or disable incremental linking

:option. machine
:target. any
:special. parse_machine
:internal.
:usage. specify target platform

:option. map
:target. any
:special. parse_map [:<file>]
:usage. generate map file

:option. nodefaultlib
:target. any
:usage. ignore default libraries

:option. noentry
:target. any
:internal.
:usage. create a DLL with no entry point

:option. nofuzzy
:target. any
:usage. disable fuzzy linking

:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:option. nologo
:target. any
:usage. don't display copyright banner

:option. opt:ref
:target. any
:enumerate. opt_level
:usage. enable dead code elimination

:option. opt:noref
:target. any
:enumerate. opt_level 
:usage. disable dead code elimination

:option. order
:target. any
:special. parse_order
:internal.
:usage. specify order of functions

:option. out
:target. any
:special. parse_out :<file>
:usage. specify output file name

:option. pdb
:target. any
:special. parse_pdb
:internal.
:usage. set PDB file name

:option. profile
:target. any
:internal.
:usage. enable profiling support in output file

:option. release
:target. any
:usage. set the checksum in the executable header

:option. section
:target. any
:special. parse_section
:internal.
:usage. set a section's attributes

:option. showwopts
:target. any
:usage. show translated options

:option. passwopts
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. nowopts
:target. any
:usage. disable default options

:option. nowref
:target. any
:usage. disable default startup module references

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. stack
:target. any
:special. parse_stack :<reserve>[,<commit>]
:usage. set stack size in bytes

:option. stub
:target. any
:special. parse_stub :<file>
:usage. attach DOS stub to program

:option. subsystem
:target. any
:special. parse_subsystem :<subsystem>
:usage. tell OS how to run the executable

:option. verbose
:target. any
:internal.
:usage. enable verbose messages

:option. version
:target. any
:special. parse_version :<major>[.<minor>]
:usage. embed version number into output file

:option. vxd
:target. any
:internal.
:usage. create a VXD

:option. warn
:target. any
:number.
:internal.
:usage. set linker warning level
