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
:cmt	22-may-96   Greg Bentz	    Adapted from rc ms.gml
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

:page. \t(Press return to continue)

:title. Usage: cvtres [options] file
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any

:noequal.

:option. help h ?
:target. any
:internal.
:usage. get help

:option. alpha
:target. any
:immediate. handle_alpha
:internal.
:usage. supplied for compatability only

:option. i386
:target. any
:immediate. handle_i386
:internal.
:usage. supplied for compatability only

:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:option. nologo
:target. any
:usage. operate quietly

:option. nowwarn
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages

:option. machine
:target. any
:special. parse_machine :<cputype>
:usage. supplied for compatability only

:option. mips
:target. any
:immediate. handle_mips
:internal.
:usage. supplied for compatability only

:option. o
:target. any
:special. parse_o  <file>
:internal.
:usage. specify output file name

:option. out
:target. any
:special. parse_out :<file>
:usage. specify output file name

:option. ppc
:target. any
:immediate. handle_ppc
:internal.
:usage. supplied for compatability only

:option. r
:target. any
:immediate. handle_r
:internal.
:usage. supplied for compatability only

:option. readonly
:target. any
:immediate. handle_readonly
:usage. supplied for compatability only

:option. showwopts
:target. any
:usage. show translated options

:option. v
:target. any
:immediate. handle_v
:internal.
:usage. supplied for compatability only

:option. verbose
:target. any
:immediate. handle_verbose
:usage. supplied for compatability only
