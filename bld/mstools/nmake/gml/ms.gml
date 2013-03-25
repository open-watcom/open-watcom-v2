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
:cmt    28-feb-96   A. Kucharczyk   Initial implementation.
:cmt	22-mar-96   Greg Bentz	    added verbose option
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

:page. \t(Press return to continue)

:title. Usage: nmake [options] [macros] [targets] [@commandfile]
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any
                                                                         

:argequal. :


:cmt    this is not really an option it is just a targets and macros storage
:option. t010101010101
:target. any
:internal.
:special. parse_t010101010101
:usage. a way to reserve storage space for tagets and macros in OPT_STORAGE

:cmt     A
:option. A
:target. any
:immediate. parse_combining
:usage. build all evaluated targets

:cmt     B
:option. B
:target. any
:immediate. parse_combining
:internal.
:usage. forces build even if timestamps are equal

:cmt     C
:option. C
:target. any
:immediate. parse_combining
:internal.
:usage. suppress output messages

:cmt     D
:option. D
:target. any
:immediate. parse_combining
:usage. display build information

:cmt     E
:option. E
:target. any
:immediate. parse_combining
:internal.
:usage. override env-var macros

:cmt     F
:option. F
:target. any
:special. parse_F <filename>
:usage. specifies filename as a makefile

:cmt     HELP
:option. HELP
:target. any
:usage. display brief usage message

:cmt     \?
:option. ?
:target. any
:usage. display brief usage message

:cmt     I
:option. I
:target. any
:immediate. parse_combining
:usage. ignore exit codes from commands

:cmt     K
:option. K
:target. any
:immediate. parse_combining
:usage. build unrelated targets on error

:cmt     L
:option. L
:target. any
:immediate. parse_combining
:internal.
:usage. don't display copyright banner

:cmt     N
:option. N
:target. any
:immediate. parse_combining
:usage. display commands but do not execute
   
:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:cmt     NOLOGO
:option. NOLOGO
:target. any
:usage. don't display copyright banner

:cmt     P
:option. P
:target. any
:immediate. parse_combining
:usage. display MAKE information

:cmt     Q
:option. Q
:target. any
:immediate. parse_combining
:usage. check time stamps but do not build

:cmt     R
:option. R
:target. any
:immediate. parse_combining
:internal.
:usage. ignore predefined rules/macros

:cmt     S
:option. S
:target. any
:immediate. parse_combining
:usage. suppress executed-commands display

:cmt     T
:option. T
:target. any
:immediate. parse_combining
:usage. change time stamps but do not build

:cmt     U
:option. U
:target. any
:immediate. parse_combining
:internal.
:usage. dump inline files

:cmt     Y
:option. Y
:target. any
:immediate. parse_combining
:internal.
:usage. disable batch-mode

:cmt     X
:option. X
:target. any
:special. parse_X <filename>
:internal.
:usage. sends NMAKE error output to filename

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. nowopts
:target. any
:usage. disable default options

:option. passwopts
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. showwopts
:target. any
:usage. show translated options

:option. verbose
:target. any
:usage. show contents of inline temporary files
