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
:cmt	Modified    By		    Reason
:cmt	--------    --		    ------
:cmt	21-nov-95   M. Hildebrand   Initial implementation.
:cmt	16-feb-96   T. Schiller	    Added /nowopts and /passwopts options.
:cmt	27-feb-96   Greg Bentz	    Added /nowwarn option.
:cmt	05-mar-96   Greg Bentz	    Added /nologo option.
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
:cmt	:immediate. <fn>			<fn> is called when option parsed
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

:title. Usage: rc [options] file [options]
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any

:noequal.


:option. c
:target. any
:special. parse_c <codepage>
:immediate handle_long_option
:internal.
:usage. select code page

:option. d
:target. any
:special. parse_d <macro>
:immediate handle_long_option
:usage. define <macro>

:option. gotlongoption_
:target. any
:internal.
:usage. used internally

:option. help h ?
:target. any
:internal.
:usage. get help

:option. fo
:target. any
:special. parse_fo <file>
:immediate handle_long_option
:usage. set output file name

:option. i
:target. any
:special. parse_i <path>
:immediate handle_long_option
:usage. add another include path

:option. l
:target. any
:special. parse_l <langid>
:immediate handle_long_option
:internal.
:usage. select language id

:option. r
:target. any
:usage. ignored, but provided for compatability

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

:option. nologo
:target. any
:usage. operate quietly

:option. nowopts
:target. any
:usage. disable default options

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. v
:target. any
:internal.
:usage. verbose operation

:option. x
:target. any
:usage. ignore INCLUDE environment variable
