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
:cmt    01-mar-96   A. Kucharczyk   Initial implementation.
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

:title. Usage: asaxp [options] <filename>
:target. any
:title. Options:
:target. any
:title. \t    ( /option is also accepted )
:target. i86 386 axp ppc
:target. any
                                                                         

:argequal. :


:cmt    this is not really an option it is just a storage for filenames
:option. t010101010101
:target. any
:internal.
:special. parse_t010101010101
:usage. a way to reserve storage space for filenames in OPT_STORAGE

:cmt    /nopp	
:option. nopp
:target. any
:internal.
:usage. do not invoke the C language preprocessor on the input file

:cmt    /D<sym>[=string]
:option. d
:target. any
:special. parse_D <sym>[=string]
:usage. define constants and macros

:cmt    /Fo
:option. fo
:target. any
:immediate. handle_Fo 
:special. parse_Fo <filename>
:usage. rename object file

:cmt    /o
:option. o
:target. any
:immediate. handle_Fo 
:special. parse_Fo <filename>
:usage. rename object file

:cmt    /I<directory>
:option. i
:target. any
:special. parse_I <directory>
:usage. specifies path for include files

:cmt    /O0	     
:option. O0	
:target. any
:internal.
:usage. turn off code scheduling optimization

:cmt    /O1          
:option. O1     
:target. any
:internal.
:usage. turn on code scheduling optimization (default)

:cmt    /resumption_safe	          
:option. resumption_safe	     
:target. any
:internal.
:usage. prevent the scheduler from moving floating point operating instructions

:cmt    /symbols_aligned_0mod4	                  
:option. symbols_aligned_0mod4	             
:target. any
:internal.
:usage. symbols are longword granular (default)

:cmt    /symbols_not_aligned	                    
:option. symbols_not_aligned	               
:target. any
:internal.
:usage. no attempt is made to align symbols

:cmt    /stack_aligned_0mod8	                        
:option. stack_aligned_0mod8	                   
:target. any
:internal.
:usage. lign stack on a quadword boundary(default)

:cmt    /stack_not_aligned                            
:option. stack_not_aligned                       
:target. any
:internal.
:usage. no attempt is made to align stack

:cmt    /eflag flags	                            
:option. eflag	                       
:target. any
:number.
:internal.
:usage. encode the exception-related flags

:cmt    /QApdst	                               
:option. QApdst	                        
:target. any
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_SILENT(default)

:cmt    /QApdsg	                                
:option. QApdsg	                         
:target. any
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_SIGNAL

:cmt    /QApdsa	                                 
:option. QApdsa	                          
:target. any
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_SIGNAL_ALL

:cmt    /QApdie	                                  
:option. QApdie	                           
:target. any
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_IEEE

:cmt    /QApdca	                                   
:option. QApdca	                            
:target. any
:internal.
:usage. set the exception-related flags to EXCEPTION_MODE_CALLER 

:cmt    /U<symbol>      
:option. u
:target. any
:internal.
:special. parse_U <sym>
:usage. undefine constants and macros

:cmt    /Zd	
:option. Zd
:target. any
:internal.
:usage. emit line numbers and public symbols for debugging

:cmt    /Zi     
:option. Zi
:target. any
:internal.
:usage. emit CodeView(tm) symbolic information for windbg

:cmt    /help
:option. help
:target. any
:usage. display brief usage message

:cmt    /?
:option. ?
:target. any
:usage. display brief usage message
   
:option. noinvoke
:target. any
:usage. don't invoke the Watcom tool

:cmt    /NOLOGO
:option. nologo
:target. any
:usage. don't display copyright banner

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
