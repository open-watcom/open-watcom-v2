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
:cmt    16-feb-96   A. Kucharczyk   Initial implementation.
:cmt    21-feb-96   A. Kucharczyk   Added /nowopts and /passwopts options
:cmt                                Changed /def option to supported
:cmt	27-feb-96   Greg Bentz	    Added /nowwarn option.
:cmt	10-jun-96   M. Hildebrand   Added /nofuzzy option.
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

:title. Usage: lib [options] [files] [@commandfile]
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any
                                                                         

:argequal. :


:cmt    /DEBUGTYPE:{COFF|CV|BOTH}
:option. debugtype
:target. any
:special. parse_debugtype
:internal.
:usage. include specified type of debugging information
    
:cmt    /DEF:[filename]
:option. def
:target. any
:special. parse_def :<file>
:usage. process .def file

:cmt    /EXPORT:symbol
:option. export
:target. any
:special. parse_export :<symbol>
:usage. export a symbol

:cmt    /EXTRACT:membername
:option. extract
:target. any
:special. parse_extract :<membername>
:usage. extract member from library

:cmt    /IMPORT:[CURRENTVER=#][,][OLDCODEVER=#][,][OLDAPIVER=#]
:option. import
:target. any
:special. parse_import :<symbol>
:internal.
:usage. import a symbol


:cmt    /INCLUDE:symbol
:option. include
:target. any
:special. parse_include :<symbol>
:internal.
:usage. include a symbol

:cmt    /LIST[:filename]
:option. list
:target. any
:special. parse_list [:<file>]
:usage. generate list file

:cmt    /MAC:{INIT=symbol|TERM=symbol}
:option. mac
:target. any
:special. parse_mac
:internal.
:usage. specify Macintosh platform options

:cmt    /MACHINE:{IX86|MIPS|ALPHA|PPC|M68K|MPPC}
:option. machine
:target. any
:special. parse_machine :<IX86|ALPHA>
:usage. specify target platform

:cmt    /NAME:filename
:option. name
:target. any
:special. parse_name :<file>
:internal.
:usage. specify output file name

:cmt    /NODEFAULTLIB[:library]
:option. nodefaultlib
:target. any
:special. parse_nodefaultlib [:library]
:internal.
:usage. ignore all default libraries

:option. nofuzzy
:target. any
:usage. disable fuzzy linking

:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:cmt    /NOLOGO
:option. nologo
:target. any
:usage. don't display copyright banner

:option. nowopts
:target. any
:usage. disable default options

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:cmt    /OUT:filename
:option. out
:target. any
:special. parse_out :<file>
:usage. specify output file name

:option. passwopts
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:cmt    /REMOVE:membername
:option. remove
:target. any
:special. parse_remove :<membername>
:usage. remove member from library

:cmt    /SUBSYSTEM:{NATIVE|WINDOWS|CONSOLE|POSIX}[,#[.#]]
:option. subsystem
:target. any
:special. parse_subsystem
:internal.
:usage. tell OS how to run the executable

:option. showwopts
:target. any
:usage. show translated options

:cmt    /VERBOSE
:option. verbose
:target. any
:internal.
:usage. enable verbose messages

