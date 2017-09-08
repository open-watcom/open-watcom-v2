:cmt
:cmt GML Macros used:
:cmt
:cmt    :chain. <char> <usage>                  options that start with <char>
:cmt                                            can be chained together i.e.,
:cmt                                            -oa -ox -ot => -oaxt
:cmt    :option. <option> <synonym> ...         define an option
:cmt    :target. <arch1> <arch2> ...            valid for these architectures
:cmt    :ntarget. <arch1> <arch2> ...           not valid for these architectures
:cmt    :immediate. <fn>                        <fn> is called when option parsed
:cmt    :code. <source-code>                    <source-code> is executed when option parsed
:cmt    :enumerate. <field> [<value>]           option is one value in <name> enumeration
:cmt    :number. [<fn>] [<default>]             =<n> allowed; call <fn> to check
:cmt    :id. [<fn>]                             =<id> req'd; call <fn> to check
:cmt    :char.[<fn>]                            =<char> req'd; call <fn> to check
:cmt    :file.                                  =<file> req'd
:cmt    :path.                                  =<path> req'd
:cmt    :special. <fn> [<arg_usage_text>]       call <fn> to parse option
:cmt    :optional.                              value is optional
:cmt    :noequal.                               args can't have option '='
:cmt    :argequal. <char>                       args use <char> instead of '='
:cmt    :internal.                              option is undocumented
:cmt    :prefix.                                prefix of a :special. option
:cmt    :usage. <text>                          English usage text
:cmt    :jusage. <text>                         Japanese usage text
:cmt    :title.                                 English title usage text
:cmt    :jtitle.                                Japanese title usage text
:cmt    :page.                                  text for paging usage message
:cmt    :nochain.                               option isn't chained with other options
:cmt    :timestamp.                             kludge to record "when" an option
:cmt                                            is set so that dependencies
:cmt                                            between options can be simulated

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx, ppc, linux, sparc, haiku

:cmt    Translations are required for the :jtitle. and :jusage. tags
:cmt    if there is no text associated with the tag.

:title. Usage: wpp [options] file [options]
:jtitle. g—p•û–@: wpp [options] file [options]
:target. i86
:title. Usage: wpp386 [options] file [options]
:jtitle. g—p•û–@: wpp386 [options] file [options]
:target. 386
:title. Usage: wppaxp [options] file [options]
:jtitle. g—p•û–@: wppaxp [options] file [options]
:target. axp
:title. Usage: wppppc [options] file [options]
:jtitle. g—p•û–@: wppppc [options] file [options]
:target. ppc
:title. Options:
:jtitle. ?I?v?V?‡?“:
:target. any
:title. \t    ( /option is also accepted )
:jtitle. \t    ( /option‚àg—p‚Å‚«‚Ü‚· )
:target. i86 386 axp ppc
:ntarget. qnx linux bsd haiku
:title. \t    ( '=' is always optional, i.e., -w4 -zp4 )
:jtitle. \t    ( '='‚Íí‚ÉÈ—ª‰Â”\‚Å‚·?B‚Â‚Ü‚è -w4 -zp4 )
:target. any

:page. (Press return to continue)
:jusage. (‘±s‚·‚é‚½‚ß‚É‰½‚©?L?[‚ğ‰Ÿ‚µ‚Ä‰º‚³‚¢)

:chain. p preprocess source file
:jusage. p ?\?[?X?t?@?C?‹‚ğ‘O?—‚µ‚Ü‚·
:chain. o optimization
:jusage. o Å“K‰»

:option. 0
:target. i86
:enumerate. arch_i86
:usage. 8086 instructions
:jusage. 8086 –½—ß

:option. 1
:target. i86
:enumerate. arch_i86
:usage. 186 instructions
:jusage. 186 –½—ß

:option. 2
:target. i86
:enumerate. arch_i86
:usage. 286 instructions
:jusage. 286 –½—ß

:option. 3
:target. i86
:enumerate. arch_i86
:usage. 386 instructions
:jusage. 386 –½—ß

:option. 4
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486
:jusage. 386 –½—ß, 486—pÅ“K‰»

:option. 5
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium
:jusage. 386 –½—ß, Pentium—pÅ“K‰»

:option. 6
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium Pro
:jusage. 386 –½—ß, Pentium Pro—pÅ“K‰»

:option. 3r 3
:target. 386
:enumerate. arch_386
:usage. 386 register calling conventions
:jusage. 386 ?Œ?W?X?^ŒÄ‚Ño‚µ‹K–ñ

:option. 3s
:target. 386
:enumerate. arch_386
:usage. 386 stack calling conventions
:jusage. 386 ?X?^?b?NŒÄ‚Ño‚µ‹K–ñ

:option. 4r 4
:target. 386
:enumerate. arch_386
:usage. 486 register calling conventions
:jusage. 486 ?Œ?W?X?^ŒÄ‚Ño‚µ‹K–ñ

:option. 4s
:target. 386
:enumerate. arch_386
:usage. 486 stack calling conventions
:jusage. 486 ?X?^?b?NŒÄ‚Ño‚µ‹K–ñ

:option. 5r 5
:target. 386
:enumerate. arch_386
:usage. Pentium register calling conventions
:jusage. Pentium ?Œ?W?X?^ŒÄ‚Ño‚µ‹K–ñ

:option. 5s
:target. 386
:enumerate. arch_386
:usage. Pentium stack calling conventions
:jusage. Pentium ?X?^?b?NŒÄ‚Ño‚µ‹K–ñ

:option. 6r 6
:target. 386
:enumerate. arch_386
:usage. Pentium Pro register calling conventions
:jusage. Pentium Pro ?Œ?W?X?^ŒÄ‚Ño‚µ‹K–ñ

:option. 6s
:target. 386
:enumerate. arch_386
:usage. Pentium Pro stack calling conventions
:jusage. Pentium Pro ?X?^?b?NŒÄ‚Ño‚µ‹K–ñ

:option. as
:target. axp
:usage. assume short integers are aligned
:jusage. short ?®?”‚ª?®—ñ‚µ‚Ä‚¢‚é‚Æ‰¼’è‚µ‚Ü‚·

:option. bc
:target. any
:usage. build target is a console application
:jusage. \’z?^?[?Q?b?g‚Í?R?“?\?[?‹¥?A?v?Š?P?[?V?‡?“‚Å‚·

:option. bd
:target. any
:usage. build target is a dynamic link library (DLL)
:jusage. \’z?^?[?Q?b?g‚Í?_?C?i?~?b?N¥?Š?“?N¥?‰?C?u?‰?Š‚Å‚·(DLL)

:option. bg
:target. any
:usage. build target is a GUI application
:jusage. \’z?^?[?Q?b?g‚ÍGUI?A?v?Š?P?[?V?‡?“‚Å‚·

:option. bm
:target. any
:usage. build target is a multi-thread environment
:jusage. \’z?^?[?Q?b?g‚Í?}?‹?`?X?Œ?b?hŠÂ‹«‚Å‚·

:option. br
:target. 386 axp ppc
:usage. build target uses DLL version of C/C++ run-time library
:jusage. \’z?^?[?Q?b?g‚ÍDLL”Å‚ÌC/C++Às?‰?C?u?‰?Š‚ğg—p‚µ‚Ü‚·

:option. bw
:target. any
:usage. build target is a default windowing application
:jusage. \’z?^?[?Q?b?g‚Í?f?t?H?‹?g¥?E?B?“?h?E¥?A?v?Š?P?[?V?‡?“‚Å‚·

:option. bt
:target. any
:id.
:optional.
:usage. build target is operating system <id>
:jusage. \’z?^?[?Q?b?g‚Í?I?y?Œ?[?e?B?“?O¥?V?X?e?€ <id>

:option. d0
:target. any
:enumerate. debug_info
:timestamp.
:usage. no debugging information
:jusage. ?f?o?b?Oî•ñ‚Í‚ ‚è‚Ü‚¹‚ñ

:option. d1
:target. any
:enumerate. debug_info
:timestamp.
:usage. line number debugging information
:jusage. s”Ô†?f?o?b?Oî•ñ

:option. d2
:target. any
:enumerate. debug_info
:timestamp.
:usage. symbolic debugging information
:jusage. Š®‘S?V?“?{?‹?f?o?b?Oî•ñ

:option. d2i
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 and debug inlines; emit inlines as COMDATs
:jusage. ?C?“?‰?C?“ŠÖ?”‚Ì“WŠJ‚È‚µ‚Ì-d2;?C?“?‰?C?“ŠÖ?”‚ÍCOMDAT‚Æ‚µ‚Äo—Í

:option. d2s
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 and debug inlines; emit inlines as statics
:jusage. ?C?“?‰?C?“ŠÖ?”‚Ì“WŠJ‚È‚µ‚Ì-d2;?C?“?‰?C?“ŠÖ?”‚Ístatic‚Æ‚µ‚Äo—Í

:option. d2t d2~
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 but without type names
:jusage. Œ^–¼‚È‚µ‚ÌŠ®‘S?V?“?{?‹?f?o?b?Oî•ñ

:option. d3
:target. any
:enumerate. debug_info
:timestamp.
:usage. symbolic debugging information with unreferenced type names
:jusage. QÆ‚³‚ê‚Ä‚¢‚È‚¢Œ^–¼‚ğŠÜ‚ŞŠ®‘S?V?“?{?‹?f?o?b?Oî•ñ

:option. d3i
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d3 and debug inlines; emit inlines as COMDATs
:jusage. ?C?“?‰?C?“ŠÖ?”‚Ì“WŠJ‚È‚µ‚Ì-d3;?C?“?‰?C?“ŠÖ?”‚ÍCOMDAT‚Æ‚µ‚Äo—Í

:option. d3s
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d3 and debug inlines; emit inlines as statics
:jusage. ?C?“?‰?C?“ŠÖ?”‚Ì“WŠJ‚È‚µ‚Ì-d3;?C?“?‰?C?“ŠÖ?”‚Ístatic‚Æ‚µ‚Äo—Í

:option. d+
:target. any
:special. scanDefinePlus
:usage. allow extended -d macro definitions
:jusage. Šg’£‚³‚ê‚½ -d ?}?N?’è‹`‚ğ‹–‰Â‚µ‚Ü‚·

:option. db
:target. any
:prefix.
:usage. generate browsing information
:jusage. ?u?‰?E?Yî•ñ‚ğ?¶?¬‚µ‚Ü‚·

:option. d
:target. any
:special. scanDefine <name>[=text]
:usage. same as #define name [text] before compilation
:jusage. ?R?“?p?C?‹‘O‚Ì #define name [text] ‚Æ“¯‚¶

:option. ecc
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __cdecl
:jusage.

:option. ecd
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __stdcall
:jusage.

:option. ecf
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __fastcall
:jusage.

:option. eco
:target. i86 386
:enumerate. intel_call_conv
:internal.
:usage. set default calling convention to _Optlink
:jusage.

:option. ecp
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __pascal
:jusage.

:option. ecr
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __fortran
:jusage.

:option. ecs
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __syscall
:jusage.

:option. ecw
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __watcall (default)
:jusage.

:option. ee
:target. any
:usage. call epilogue hook routine
:jusage. ?G?s??[?O¥?t?b?N?‹?[?`?“‚ğŒÄ‚Ño‚µ‚Ü‚·

:option. ef
:target. any
:usage. use full path names in error messages
:jusage. ?G?‰?[???b?Z?[?W‚ÉŠ®‘S?p?X–¼‚ğg—p‚µ‚Ü‚·

:option. ei
:target. any
:enumerate. enum_size
:usage. force enum base type to use at least an int
:jusage. enumŒ^‚Ì?x?[?XŒ^‚Æ‚µ‚ÄintŒ^?Èã‚Ì‘å‚«‚³‚ğg—p‚µ‚Ü‚·

:option. em
:target. any
:enumerate. enum_size
:usage. force enum base type to use minimum integral type
:jusage. enumŒ^‚Ì?x?[?XŒ^‚Æ‚µ‚ÄÅ¬‚Ì?®?”Œ^‚ğg—p‚µ‚Ü‚·

:option. en
:target. any
:usage. emit routine names in the code segment
:jusage. ?‹?[?`?“–¼‚ğ?R?[?h?Z?O???“?g‚Éo—Í‚µ‚Ü‚·

:option. ep
:target. any
:number. checkPrologSize 0
:usage. call prologue hook routine with <num> stack bytes available
:jusage. <num>?o?C?g‚Ì?X?^?b?N‚ğg—p‚·‚é?v???[?O¥?t?b?N¥?‹?[?`?“‚ğŒÄ‚Ño‚µ‚Ü‚·

:option. eq
:target. any
:immediate. handleOptionEQ
:usage. do not display error messages (but still write to .err file)
:jusage. ?G?‰?[???b?Z?[?W‚ğ•\¦‚µ‚Ü‚¹‚ñ(‚µ‚©‚µ.err?t?@?C?‹‚É‚Í‘‚«‚İ‚Ü‚·)

:option. er
:target. any
:usage. do not recover from undefined symbol errors
:jusage. –¢’è‹`?V?“?{?‹?G?‰?[‚©‚ç‰ñ•œ‚µ‚Ü‚¹‚ñ

:option. et
:target. 386
:usage. emit Pentium profiling code
:jusage. Pentium?v??t?@?C?Š?“?O¥?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. et0
:target. 386
:usage. emit Pentium-CTR0 profiling code
:jusage. Pentium-CTR0?v??t?@?C?Š?“?O¥?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. etp
:target. 386
:internal.
:usage. emit Timing for Profiler
:jusage. ?v??t?@?C?‰‚Ì?^?C?~?“?O‚ğo—Í‚µ‚Ü‚·

:option. esp
:target. 386
:internal.
:usage. emit Statement counting for Profiler
:jusage. ?v??t?@?C?‰—p?X?e?[?g???“?g¥?J?E?“?e?B?“?O‚ğo—Í‚µ‚Ü‚·

:option. ew
:target. any
:immediate. handleOptionEW
:usage. alternate error message formatting
:jusage. •Ê‚Ì?G?‰?[???b?Z?[?WŒ`®‚ğg—p‚µ‚Ü‚·

:option. ez
:target. 386
:usage. generate PharLap EZ-OMF object files
:jusage. PharLap EZ-OMF?I?u?W?F?N?g¥?t?@?C?‹‚ğ?¶?¬‚µ‚Ü‚·

:option. e
:target. any
:number. checkErrorLimit
:usage. set limit on number of error messages
:jusage. ?G?‰?[???b?Z?[?W?”‚Ì?§ŒÀ‚ğ?İ’è‚µ‚Ü‚·

:option. fbi
:target. any
:special. scanFBI
:internal.
:usage. generate browsing information
:jusage. ?u?‰?E?Yî•ñ‚ğ?¶?¬‚µ‚Ü‚·

:option. fbx
:target. any
:special. scanFBX
:internal.
:usage. do not generate browsing information
:jusage. ?u?‰?E?Yî•ñ‚ğ?¶?¬‚µ‚Ü‚¹‚ñ

:option. fc
:target. any
:file.
:immediate. handleOptionFC
:usage. specify file of command lines to be batch processed
:jusage. ?o?b?`?—‚·‚é?R?}?“?h?‰?C?“‚Ì?t?@?C?‹‚ğw’è‚µ‚Ü‚·

:option. fh
:target. any
:file.
:optional.
:timestamp.
:usage. use pre-compiled header (PCH) file
:jusage. ?v?Š?R?“?p?C?‹¥?w?b?_?[(PCH)‚ğg—p‚µ‚Ü‚·

:option. fhd
:target. any
:usage. store debug info for PCH once (DWARF only)
:jusage. PCH—p?f?o?b?Oî•ñ‚ğ1“xŠi”[‚µ‚Ü‚·(DWARF‚Ì‚İ)

:option. fhq
:target. any
:file.
:optional.
:timestamp.
:usage. do not display PCH activity warnings
:jusage. PCHg—p???b?Z?[?W‚ğ•\¦‚µ‚Ü‚¹‚ñ

:option. fhw
:target. any
:usage. force compiler to write PCH (will never read)
:jusage. PCH‚ğ‘‚«‚İ‚É‹­?§‚µ‚Ü‚· (“Ç‚İ‚Ü‚¹‚ñ)

:option. fhwe
:target. any
:usage. don't count PCH activity warnings (see -we option)
:jusage. PCHg—p???b?Z?[?W‚ğ?G?‰?[‚Æ‚µ‚Ä?µ‚¢‚Ü‚¹‚ñ(-we?I?v?V?‡?“QÆ)

:option. fhr
:target. any
:usage. force compiler to read PCH (will never write)
:jusage. PCH‚ğ“Ç‚İ‚İ‚É‹­?§‚µ‚Ü‚· (‘‚«‚Ü‚¹‚ñ)

:option. fhr!
:target. any
:usage. compiler will read PCH without checking include files
:jusage. ?R?“?p?C?‰‚Í?C?“?N?‹?[?h?t?@?C?‹‚ğ?`?F?b?N‚·‚é‚±‚Æ‚È‚­?APCH“Ç‚İ‚İ‚Ü‚·
:internal.

:option. fi
:target. any
:file.
:usage. force <file> to be included
:jusage. ‹­?§“I‚É<file>‚ğ?C?“?N?‹?[?h‚µ‚Ü‚·

:option. fip
:target. any
:file.
:optional.
:usage. automatic inclusion of <file> instead of _preincl.h (default)
:jusage.

:option. fo
:target. any
:file.
:optional.
:usage. set object or preprocessor output file name
:jusage. ?I?u?W?F?N?g‚Ü‚½‚Í?v?Š?v??Z?b?T‚Ìo—Í?t?@?C?‹–¼‚ğ?İ’è‚µ‚Ü‚·

:option. fr
:target. any
:file.
:optional.
:usage. set error file name
:jusage. ?G?‰?[¥?t?@?C?‹–¼‚ğ?İ’è‚µ‚Ü‚·

:option. ft
:target. any
:enumerate. file_83
:usage. check for truncated versions of file names
:jusage. ?Ø‚è‹l‚ß‚½?t?@?C?‹–¼‚ğ?`?F?b?N‚µ‚Ü‚·

:option. fx
:target. any
:enumerate. file_83
:usage. do not check for truncated versions of file names
:jusage. ?Ø‚è‹l‚ß‚½?t?@?C?‹–¼‚ğ?`?F?b?N‚µ‚Ü‚¹‚ñ

:option. fzh
:target. any
:usage. do not automatically postfix include file names
:jusage. do not automatically postfix include file names

:option. fzs
:target. any
:usage. do not automatically postfix source file names
:jusage. do not automatically postfix source file names

:option. 87d
:target. i86 386
:number. CmdX86CheckStack87 0
:enumerate. intel_fpu_model
:internal.
:usage. inline 80x87 instructions with specified depth
:jusage. w’è‚µ‚½?[‚³‚Ì?C?“?‰?C?“80x87–½—ß

:option. fpc
:target. i86 386
:enumerate. intel_fpu_model
:usage. calls to floating-point library
:jusage. •‚“®¬?”“_?‰?C?u?‰?Š‚ğŒÄ‚Ño‚µ‚Ü‚·

:option. fpi
:target. i86 386
:enumerate. intel_fpu_model
:usage. inline 80x87 instructions with emulation
:jusage. ?G?~?…?Œ?[?V?‡?“•t‚«?C?“?‰?C?“80x87–½—ß

:option. fpi87
:target. i86 386
:enumerate. intel_fpu_model
:usage. inline 80x87 instructions
:jusage. ?C?“?‰?C?“80x87–½—ß

:option. fp2 fp287
:target. i86 386
:enumerate. intel_fpu_level
:usage. generate 287 floating-point code
:jusage. 287•‚“®¬?”“_?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. fp3 fp387
:target. i86 386
:enumerate. intel_fpu_level
:usage. generate 387 floating-point code
:jusage. 387•‚“®¬?”“_?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. fp5
:target. i86 386
:enumerate. intel_fpu_level
:usage. optimize floating-point for Pentium
:jusage. Pentium—p•‚“®¬?”“_Å“K‰»‚ğ‚µ‚Ü‚·

:option. fp6
:target. i86 386
:enumerate. intel_fpu_level
:usage. optimize floating-point for Pentium Pro
:jusage. Pentium Pro—p•‚“®¬?”“_Å“K‰»‚ğ‚µ‚Ü‚·

:option. fpr
:target. i86 386
:usage. generate backward compatible 80x87 code
:jusage. ?o?[?W?‡?“9.0?È‘O‚ÆŒİŠ·‚Ì80x87?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. fpd
:target. i86 386
:usage. enable Pentium FDIV check
:jusage. Pentium FDIV?`?F?b?N‚ğ‚µ‚Ü‚·

:option. g
:target. i86 386
:id.
:usage. set code group name
:jusage. ?R?[?h¥?O?‹?[?v–¼‚ğ?İ’è‚µ‚Ü‚·

:option. hw
:target. i86 386 
:enumerate. dbg_output
:usage. generate Watcom debugging information
:jusage. Watcom?f?o?b?Oî•ñ‚ğ?¶?¬‚µ‚Ü‚·

:option. hd
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate DWARF debugging information
:jusage. DWARF?f?o?b?Oî•ñ‚ğ?¶?¬‚µ‚Ü‚·

:option. hda
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate DWARF debugging information
:jusage. DWARF?f?o?b?Oî•ñ‚ğ?¶?¬‚µ‚Ü‚·
:internal.

:option. hc
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate Codeview debugging information
:jusage. Codeview?f?o?b?Oî•ñ‚ğ?¶?¬‚µ‚Ü‚·

:option. i
:target. any
:path.
:usage. add another include path
:jusage. ?C?“?N?‹?[?h¥?p?X‚ğ’Ç‰Á‚µ‚Ü‚·

:option. j
:target. any
:usage. change char default from unsigned to signed
:jusage. charŒ^‚Ì?f?t?H?‹?g‚ğunsigned‚©‚çsigned‚É•ÏX‚µ‚Ü‚·

:option. jw
:target. any
:usage. warn about default char promotion to int
:jusage.
:internal.

:option. k
:target. any
:usage. continue processing files (ignore errors)
:jusage. ?t?@?C?‹?—‚ğ‘±s‚µ‚Ü‚·(?G?‰?[‚ğ–³‹‚µ‚Ü‚·)

:option. la
:target. axp
:usage. output Alpha assembly listing
:jusage. Alpha?A?Z?“?u?Š¥?Š?X?g‚ğo—Í‚µ‚Ü‚·
:internal.

:option. lo
:target. axp
:usage. output OWL listing
:jusage. OWL?Š?X?g‚ğo—Í‚µ‚Ü‚·
:internal.

:option. mc
:target. i86 386
:enumerate. mem_model
:usage. compact memory model (small code/large data)
:jusage. ?R?“?p?N?g¥???‚?Š¥?‚?f?‹(?X?‚?[?‹¥?R?[?h/?‰?[?W¥?f?[?^)

:option. mf
:target. 386
:enumerate. mem_model
:usage. flat memory model (small code/small data assuming CS=DS=SS=ES)
:jusage. ?t?‰?b?g¥???‚?Š¥?‚?f?‹(?X?‚?[?‹¥?R?[?h/CS=DS=SS=ES‚ğ‰¼’è‚µ‚½?X?‚?[?‹¥?f?[?^)

:option. mfi
:target. 386
:enumerate. mem_model
:usage. flat memory model (interrupt functions will assume flat model)
:jusage. ?t?‰;?g¥???‚?Š¥?‚??‹(Š„‚è‚İŠÖ?”‚à?t?‰;?g?‚??‹‚Å‚ ‚é‚Æ‰¼’è‚·‚é)

:option. mh
:target. i86
:enumerate. mem_model
:usage. huge memory model (large code/huge data)
:jusage. ?q?…?[?W¥???‚?Š¥?‚?f?‹(?‰?[?W¥?R?[?h/?q?…?[?W¥?f?[?^)

:option. ml
:target. i86 386
:enumerate. mem_model
:usage. large memory model (large code/large data)
:jusage. ?‰?[?W¥???‚?Š¥?‚?f?‹(?‰?[?W¥?R?[?h/?‰?[?W¥?f?[?^)

:option. mm
:target. i86 386
:enumerate. mem_model
:usage. medium memory model (large code/small data)
:jusage. ?~?f?B?A?€¥???‚?Š¥?‚?f?‹(?‰?[?W¥?R?[?h/?X?‚?[?‹¥?f?[?^)

:option. ms
:target. i86 386
:enumerate. mem_model
:usage. small memory model (small code/small data)
:jusage. ?X?‚?[?‹¥???‚?Š¥?‚?f?‹(?X?‚?[?‹¥?R?[?h/?X?‚?[?‹¥?f?[?^)

:option. nc
:target. i86 386
:id.
:usage. set code class name
:jusage. ?R?[?h¥?N?‰?X–¼‚ğ?İ’è‚µ‚Ü‚·

:option. nd
:target. i86 386
:id.
:usage. set data segment name
:jusage. ?f?[?^¥?Z?O???“?g–¼‚ğ?İ’è‚µ‚Ü‚·

:option. nm
:target. i86 386 axp ppc
:file.
:usage. set module name
:jusage. ?‚?W?…?[?‹–¼‚ğ?İ’è‚µ‚Ü‚·

:option. nt
:target. i86 386
:id.
:usage. set name of text segment
:jusage. ?e?L?X?g¥?Z?O???“?g–¼‚ğ?İ’è‚µ‚Ü‚·

:option. oa
:target. any
:usage. relax aliasing constraints
:jusage. ?G?C?Š?A?X‚Ì?§–ñ‚ğŠÉ?a‚µ‚Ü‚·

:option. ob
:target. any
:usage. enable branch prediction
:jusage. •ªŠò—\‘ª‚É‚»‚Á‚½?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. oc
:target. i86 386 axp ppc
:usage. disable <call followed by return> to <jump> optimization
:jusage. <call followed by return>‚©‚ç<jump>‚ÌÅ“K‰»‚ğ–³Œø‚É‚µ‚Ü‚·

:option. od
:target. any
:enumerate. opt_level
:timestamp.
:usage. disable all optimizations
:jusage. ‚·‚×‚Ä‚ÌÅ“K‰»‚ğ–³Œø‚É‚µ‚Ü‚·

:option. oe
:target. any
:number. checkOENumber 100
:usage. expand user functions inline (<num> controls max size)
:jusage. ?†?[?UŠÖ?”‚ğ?C?“?‰?C?““WŠJ‚µ‚Ü‚·(<num>‚ÍÅ‘å»²½Ş‚ğ?§Œä‚µ‚Ü‚·)

:option. of
:target. i86 386
:usage. generate traceable stack frames as needed
:jusage. •K—v‚É‰‚¶‚Ä?g?Œ?[?X‰Â”\‚È?X?^?b?N¥?t?Œ?[?€‚ğ?¶?¬‚µ‚Ü‚·

:option. of+
:target. i86 386
:usage. always generate traceable stack frames
:jusage. í‚É?g?Œ?[?X‰Â”\‚È?X?^?b?N¥?t?Œ?[?€‚ğ?¶?¬‚µ‚Ü‚·

:option. oh
:target. any
:usage. enable expensive optimizations (longer compiles)
:jusage. Å“K‰»‚ğŒJ‚è•Ô‚µ‚Ü‚·(?R?“?p?C?‹‚ª’·‚­‚È‚è‚Ü‚·)

:option. oi
:target. any
:usage. expand intrinsic functions inline
:jusage. ‘g‚İŠÖ?”‚ğ?C?“?‰?C?““WŠJ‚µ‚Ü‚·

:option. oi+
:target. any
:usage. enable maximum inlining depth
:jusage. ?C?“?‰?C?““WŠJ‚Ì?[‚³‚ğÅ‘å‚É‚µ‚Ü‚·

:option. ok
:target. any
:usage. include prologue/epilogue in flow graph
:jusage. ?v???[?O‚Æ?G?s??[?O‚ğ?t??[?§Œä‰Â”\‚É‚µ‚Ü‚·

:option. ol
:target. any
:usage. enable loop optimizations
:jusage. ?‹?[?vÅ“K‰»‚ğ‰Â”\‚É‚µ‚Ü‚·

:option. ol+
:target. any
:usage. enable loop unrolling optimizations
:jusage. ?‹?[?v?E?A?“??[?Š?“?O‚Å?‹?[?vÅ“K‰»‚ğ‰Â”\‚É‚µ‚Ü‚·

:option. om
:target. i86 386 axp ppc
:usage. generate inline code for math functions
:jusage. ZpŠÖ?”‚ğ?C?“?‰?C?“‚Ì80x87?R?[?h‚Å“WŠJ‚µ‚Ä?¶?¬‚µ‚Ü‚·

:option. on
:target. any
:usage. allow numerically unstable optimizations
:jusage. ?”’l“I‚É‚â‚â•s?³Šm‚É‚È‚é‚ª‚æ‚è‚‘¬‚ÈÅ“K‰»‚ğ‰Â”\‚É‚µ‚Ü‚·

:option. oo
:target. any
:usage. continue compilation if low on memory
:jusage. ???‚?Š‚ª‘«‚è‚È‚­‚È‚Á‚Ä‚à?R?“?p?C?‹‚ğŒp‘±‚µ‚Ü‚·

:option. op
:target. any
:usage. generate consistent floating-point results
:jusage. ?êŠÑ‚µ‚½•‚“®¬?”“_ŒvZ‚ÌŒ‹‰Ê‚ğ?¶?¬‚µ‚Ü‚·

:option. or
:target. any
:usage. reorder instructions for best pipeline usage
:jusage. Å“K‚È?p?C?v?‰?C?“‚ğg—p‚·‚é‚½‚ß‚É–½—ß‚ğ•À‚×‘Ö‚¦‚Ü‚·

:option. os
:target. any
:enumerate. opt_size_time
:timestamp.
:usage. favor code size over execution time in optimizations
:jusage. ÀsŠÔ‚æ‚è?R?[?h?T?C?Y‚ÌÅ“K‰»‚ğ—D?æ‚µ‚Ü‚·

:option. ot
:target. any
:enumerate. opt_size_time
:timestamp.
:usage. favor execution time over code size in optimizations
:jusage. ?R?[?h?T?C?Y‚æ‚èÀsŠÔ‚ÌÅ“K‰»‚ğ—D?æ‚µ‚Ü‚·

:option. ou
:target. any
:usage. all functions must have unique addresses
:jusage. ‚·‚×‚Ä‚ÌŠÖ?”‚Í‚»‚ê‚¼‚êŒÅ—L‚Ì?A?h?Œ?X‚ğ•K‚¸‚¿‚Ü‚·

:option. ox
:target. any
:enumerate. opt_level
:timestamp.
:usage. equivalent to -obmiler -s
:jusage. -obmiler -s‚Æ“¯“™

:option. oz
:target. any
:usage. NULL points to valid memory in the target environment
:jusage. NULL‚Í?A?^?[?Q?b?gŠÂ‹«“à‚Ì—LŒø‚È???‚?Š‚ğw‚µ‚Ü‚·

:option. ad
:target. any
:file.
:optional.
:usage. generate make style automatic dependency file
:jusage. generate make style automatic dependency file

:option. adbs
:target. any
:usage. force path separators to '\\' in auto-depend file
:jusage. force path separators to '\\' in auto-depend file

:option. adfs
:target. any
:usage. force path separators to '/' in auto-depend file
:jusage. force path separators to '/' in auto-depend file

:option. add
:target. any
:file.
:optional.
:usage. specify first dependency in make style auto-depend file
:jusage. specify first dependency in make style auto-depend file

:option. adhp
:target. any
:file.
:optional.
:usage. specify default path for headers without one
:jusage. specify default path for headers without one

:option. adt
:target. any
:file.
:optional.
:usage. specify target in make style auto-depend file
:jusage. specify target in make style auto-depend file

:option. pil
:target. any
:nochain.
:usage. preprocessor ignores #line directives
:jusage. preprocessor ignores #line directives

:option. p
:target. any
:usage.
:jusage.

:option. pe
:target. any
:usage. encrypt identifiers
:jusage. ¯•Êq‚ğ?Ã†‰»‚µ‚Ü‚·

:option. pl
:target. any
:usage. insert #line directives
:jusage. #line‹[—–½—ß‚ğ‘}“ü‚µ‚Ü‚·

:option. pj
:target. any
:internal.
:usage. insert // #line comments
:jusage. // #line‹[—–½—ß‚ğ‘}“ü‚µ‚Ü‚·

:option. pc
:target. any
:usage. preserve comments
:jusage. ?R???“?g‚ğc‚µ‚Ü‚·

:option. pw
:target. any
:number. checkPPWidth
:usage. wrap output lines at <num> columns. Zero means no wrap.
:jusage. o—Ís‚ğ<num>Œ…‚Å?Ü‚è•Ô‚µ‚Ü‚·. 0‚Í?Ü‚è•Ô‚µ‚Ü‚¹‚ñ.

:option. p#
:target. any
:char.
:internal.
:usage. set preprocessor delimiter to something other than '#'
:jusage. ?v?Š?v??Z?b?T‚Ì‹æ?Ø‚è‹L†‚ğ'#'?ÈŠO‚Ì‰½‚©‚É?İ’è‚µ‚Ü‚·

:option. q
:target. any
:usage. operate quietly (display only error messages)
:jusage. –³???b?Z?[?W?‚?[?h‚Å“®ì‚µ‚Ü‚·(?G?‰?[???b?Z?[?W‚Ì‚İ•\¦‚³‚ê‚Ü‚·)

:option. r
:target. i86 386
:usage. save/restore segment registers across calls
:jusage. ŠÖ?”ŒÄ‚Ño‚µ‚Ì‘OŒã‚Å?Z?O???“?g?Œ?W?X?^‚ğ‘Ş”ğ/?Š?X?g?A‚µ‚Ü‚·

:option. re
:target. 386
:internal.
:usage. special Neutrino R/W data access code generation
:jusage.

:option. ri
:target. i86 386
:usage. return chars and shorts as ints
:jusage. ‘S‚Ä‚ÌŠÖ?”‚Ì?ø?”‚Æ–ß‚è’l‚ğintŒ^‚É•ÏŠ·‚µ‚Ü‚·

:option. rod
:target. any
:path.
:internal.
:usage. specified <path> contains read-only files
:jusage. w’è‚³‚ê‚½<path>‚É‚Í“Ç‚İ‚İ?ê—p?t?@?C?‹‚ªŠÜ‚Ü‚ê‚Ä‚¢‚Ü‚·

:option. s
:target. any
:usage. remove stack overflow checks
:jusage. ?X?^?b?N?I?[?o?t??[?E?`?F?b?N‚ğíœ‚µ‚Ü‚·

:option. sg
:target. i86 386
:usage. generate calls to grow the stack
:jusage. ?X?^?b?N‚ğ‘‰Á‚·‚éŒÄ‚Ño‚µ‚ğ?¶?¬‚µ‚Ü‚·

:option. si
:target. axp
:usage. generate calls to initialize local storage
:jusage. ??[?J?‹¥???‚?Š‚ğ‰Šú‰»‚·‚éŒÄ‚Ño‚µ‚ğ?¶?¬‚µ‚Ü‚·

:option. st
:target. i86 386
:usage. touch stack through SS first
:jusage. ‚Ü‚¸Å‰‚ÉSS‚ğ’Ê‚µ‚Ä?X?^?b?N?E?^?b?`‚µ‚Ü‚·

:option. t
:target. any
:number. checkTabWidth
:usage. set number of spaces in a tab stop
:jusage. ?^?u¥?X?g?b?v‚É‘Î‰‚·‚é‹ó”’‚Ì?”‚ğ?İ’è‚µ‚Ü‚·

:option. tp
:target. dbg
:id.
:usage. set #pragma on( <id> )
:jusage. #pragma on( <id> )‚ğ?İ’è‚µ‚Ü‚·

:option. u
:target. any
:special. scanUndefine [=<name>]
:usage. undefine macro name
:jusage. ?}?N?–¼‚ğ–¢’è‹`‚É‚µ‚Ü‚·

:option. v
:target. any
:usage. output function declarations to .def file
:jusage. .def?t?@?C?‹‚ÉŠÖ?”?éŒ¾‚ğo—Í‚µ‚Ü‚·

:option. vcap
:target. 386 axp
:usage. VC++ compatibility: alloca allowed in argument lists
:jusage. VC++ ŒİŠ·?«: ?ø?”?Š?X?g‚Ì’†‚Åalloca‚ğg—p‚Å‚«‚Ü‚·

:option. w
:target. any
:enumerate. warn_level
:number. checkWarnLevel
:usage. set warning level number
:jusage. Œx??Œ?x?‹”Ô†‚ğ?İ’è‚µ‚Ü‚·

:option. wcd
:target. any
:number.
:multiple.
:usage. warning control: disable warning message <num>
:jusage. Œx??§Œä: Œx????b?Z?[?W<num>‚ğ‹Ö~‚µ‚Ü‚·

:option. wce
:target. any
:number.
:multiple.
:usage. warning control: enable warning message <num>
:jusage. Œx??§Œä: Œx????b?Z?[?W <num> ‚Ì•\¦‚ğ‚µ‚Ü‚·

:option. we
:target. any
:usage. treat all warnings as errors
:jusage. ‚·‚×‚Ä‚ÌŒx?‚ğ?G?‰?[‚Æ‚µ‚Ä?µ‚¢‚Ü‚·

:option. wpx
:target. any
:internal.
:usage. internal experimental option, check prototypes defined already
:jusage.

:option. wx
:target. any
:enumerate. warn_level
:usage. set warning level to maximum setting
:jusage. Œx??Œ?x?‹‚ğÅ‘å?İ’è‚É‚µ‚Ü‚·

:option. x
:target. any
:usage. ignore INCLUDE environment variable
:jusage. ignore INCLUDE environment variable

:option. xbnm
:target. any
:internal.
:usage. use latest (incompatible) name mangling algorithms
:jusage. 

:option. xbsa
:target. any
:internal.
:usage. do not align segments if at all possible
:jusage. 

:option. xbov1
:target. any
:internal.
:usage. WP 13.3.3.2 change
:jusage. 

:option. xcmb
:target. any
:internal.
:usage. bind modifiers during template instantiation
:jusage. ?e?“?v?Œ?[?g‚ğ?C?“?X?^?“?X‰»‚·‚éŠÔ‚ÉCüq‚ğ?o?C?“?h‚µ‚Ü‚·

:option. xcpi
:target. any
:internal.
:usage. instantiate template functions if a prototype is visible
:jusage. ?v??g?^?C?v‚ª‚ ‚ê‚Î?A?e?“?v?Œ?[?gŠÖ?”‚ğ?C?“?X?^?“?X‰»‚µ‚Ü‚·

:option. xd
:target. any
:enumerate. exc_level
:usage. disable exception handling (default) 
:jusage. —áŠO?—‚ğg—p•s‰Â‚É‚µ‚Ü‚·(?f?t?H?‹?g) 

:option. xds
:target. any
:enumerate. exc_level
:usage. disable exception handling (table-driven destructors)
:jusage. —áŠO?—‚ğg—p•s‰Â‚É‚µ‚Ü‚·(?e?[?u?‹¥?h?Š?u?“‚Ì?f?X?g?‰?N?^)

:option. xdt
:target. any
:enumerate. exc_level
:usage. disable exception handling (same as -xd)
:jusage. —áŠO?—‚ğg—p•s‰Â‚É‚µ‚Ü‚·(-xd‚Æ“¯‚¶)

:option. xr
:target. any
:usage. enable RTTI
:jusage. RTTI‚ğg—p‰Â”\‚É‚µ‚Ü‚·

:option. xs
:target. any
:enumerate. exc_level
:usage. enable exception handling 
:jusage. —áŠO?—‚ğg—p‰Â”\‚É‚µ‚Ü‚·

:option. xss
:target. any
:enumerate. exc_level
:usage. enable exception handling (table-driven destructors)
:jusage. —áŠO?—‚ğg—p‰Â”\‚É‚µ‚Ü‚·(?e?[?u?‹¥?h?Š?u?“‚Ì?f?X?g?‰?N?^)

:option. xst
:target. any
:enumerate. exc_level
:usage. enable exception handling (direct calls for destruction)
:jusage. —áŠO?—‚ğg—p‰Â”\‚É‚µ‚Ü‚·(?f?X?g?‰?N?^‚Ö‚Ì’¼?ÚŒÄo‚µ)

:option. xgls
:target. i86
:internal.
:usage. only check seg of i86 far pointers when checking NULL equality
:jusage.

:option. xgv
:target. 386
:internal.
:usage. indexed global variables
:jusage. ?C?“?f?b?N?X•t‚«?O??[?o?‹•Ï?”

:option. xto
:target. any
:internal.
:usage. obfuscate type signature names
:jusage.

:option. xx
:target. any
:nochain.
:usage. ignore default directories for file search (.,../h,../c,...)
:jusage. ignore default directories for file search (.,../h,../c,...)

:option. za
:target. any
:enumerate. iso
:usage. disable extensions (i.e., accept only ISO/ANSI C++)
:jusage. Šg’£‹@”\‚ğg—p•s‰Â‚É‚µ‚Ü‚·(‚Â‚Ü‚è, ISO/ANSI C++‚Ì‚İó‚¯•t‚¯‚Ü‚·)

:option. za0x
:target. any
:usage. enable some features of the upcoming ISO C++0x standard
:jusage.

:option. zam
:target. any
:usage. disable all predefined non-ISO extension macros
:jusage. disable all predefined non-ISO extension macros

:option. zat
:target. any
:usage. disable alternative tokens (e.g. and, or, not)
:jusage.

:option. zc
:target. i86 386
:usage. place const data into the code segment
:jusage. ?Š?e?‰?‹•¶š—ñ‚ğ?R?[?h?Z?O???“?g‚É“ü‚ê‚Ü‚·

:option. zdf
:target. i86 386
:enumerate. ds_peg
:usage. DS floats (i.e. not fixed to DGROUP)
:jusage. DS‚ğ•‚“®‚É‚µ‚Ü‚·(‚Â‚Ü‚èDGROUP‚ÉŒÅ’è‚µ‚Ü‚¹‚ñ)

:option. zdp
:target. i86 386
:enumerate. ds_peg
:usage. DS is pegged to DGROUP
:jusage. DS‚ğDGROUP‚ÉŒÅ’è‚µ‚Ü‚·

:option. zdl
:target. 386
:usage. load DS directly from DGROUP
:jusage. DGROUP‚©‚çDS‚É’¼?Ú??[?h‚µ‚Ü‚·

:option. ze
:target. any
:enumerate. iso
:usage. enable extensions (i.e., near, far, export, etc.)
:jusage. Šg’£‹@”\‚ğg—p‰Â”\‚É‚µ‚Ü‚·(‚Â‚Ü‚è, near, far, export, “™.)

:option. zf
:target. any
:usage. scope of for loop initializer extends beyond loop
:jusage. FIX ME

:option. zfw
:target. i86
:usage. generate FWAIT instructions on 386 and later
:jusage.

:option. zfw
:target. 386
:usage. generate FWAIT instructions
:jusage.

:option. zff
:target. i86 386
:enumerate. fs_peg
:usage. FS floats (i.e. not fixed to a segment)
:jusage. FS‚ğ•‚“®‚É‚µ‚Ü‚·(‚Â‚Ü‚è, 1‚Â‚Ì?Z?O???“?g‚ÉŒÅ’è‚µ‚Ü‚¹‚ñ)

:option. zfp
:target. i86 386
:enumerate. fs_peg
:usage. FS is pegged to a segment
:jusage. FS‚ğ1‚Â‚Ì?Z?O???“?g‚ÉŒÅ’è‚µ‚Ü‚·

:option. zg
:target. any
:usage. generate function prototypes using base types
:jusage. Šî–{Œ^‚ğg—p‚µ‚½ŠÖ?”?v??g?^?C?v‚ğ?¶?¬‚µ‚Ü‚·

:option. zgf
:target. i86 386
:enumerate. gs_peg
:usage. GS floats (i.e. not fixed to a segment)
:jusage. GS‚ğ•‚“®‚É‚µ‚Ü‚·(‚Â‚Ü‚è, 1‚Â‚Ì?Z?O???“?g‚ÉŒÅ’è‚µ‚Ü‚¹‚ñ)

:option. zgp
:target. i86 386
:enumerate. gs_peg
:usage. GS is pegged to a segment
:jusage. GS‚ğ1‚Â‚Ì?Z?O???“?g‚ÉŒÅ’è‚µ‚Ü‚·

:option. zi
:target. dbg
:usage. dump informational statistics to stdout
:jusage. î•ñ‚Æ‚µ‚Ä“Œv’l‚ğstdout‚Éo—Í‚µ‚Ü‚·

:option. zk0 zk
:target. any
:enumerate. char_set
:usage. double-byte character support: Kanji
:jusage. 2?o?C?g•¶š?T?|?[?g: “ú–{Œê

:option. zk1
:target. any
:enumerate. char_set
:usage. double-byte character support: Chinese/Taiwanese
:jusage. 2?o?C?g•¶š?T?|?[?g: ’†‘Œê/‘ä?pŒê

:option. zk2
:target. any
:enumerate. char_set
:usage. double-byte character support: Korean
:jusage. 2?o?C?g•¶š?T?|?[?g: ŠØ‘Œê

:option. zk0u
:target. any
:enumerate. char_set
:usage. translate double-byte Kanji to Unicode
:jusage. 2?o?C?gŠ¿š‚ğUnicode‚É•ÏŠ·‚µ‚Ü‚·

:option. zkl
:target. any
:enumerate. char_set
:usage. double-byte character support: local installed language
:jusage. 2?o?C?g•¶š?T?|?[?g: ??[?J?‹‚É?C?“?X?g?[?‹‚³‚ê‚½Œ¾Œê

:option. zku
:target. any
:enumerate. char_set
:number.
:usage. load Unicode translate table for specified code page
:jusage. w’è‚µ‚½?R?[?h?y?[?W‚ÌUnicode•ÏŠ·?e?[?u?‹‚ğ??[?h‚µ‚Ü‚·

:option. zl
:target. any
:usage. remove default library information
:jusage. ?f?t?H?‹?g¥?‰?C?u?‰?Šî•ñ‚ğíœ‚µ‚Ü‚·

:option. zld
:target. any
:usage. remove file dependency information
:jusage. ?t?@?C?‹?Ë‘¶î•ñ‚ğíœ‚µ‚Ü‚·

:option. zlf
:target. any
:usage. always generate default library information
:jusage. ?f?t?H?‹?g¥?‰?C?u?‰?Šî•ñ‚ğí‚É?¶?¬‚µ‚Ü‚·

:option. zls
:target. any
:usage. remove automatically inserted symbols
:jusage. remove automatically inserted symbols

:option. zm
:target. i86 386
:usage. emit functions in separate segments
:jusage. ŠeŠÖ?”‚ğ•Ê‚Ì?Z?O???“?g‚É“ü‚ê‚Ü‚·

:option. zm
:target. axp
:usage. emit functions in separate sections
:jusage. ŠeŠÖ?”‚ğ•Ê‚Ì?Z?O???“?g‚É“ü‚ê‚Ü‚·

:option. zmf
:target. i86 386
:usage. emit functions in separate segments (near functions allowed)
:jusage. ŠeŠÖ?”‚ğ•Ê‚Ì?Z?O???“?g‚É“ü‚ê‚Ü‚·(nearŠÖ?”‚ª‰Â”\‚Å‚·)

:option. zo
:target. 386
:usage. use exception-handling for a specific operating system
:jusage. w’è‚³‚ê‚½?I?y?Œ?[?e?B?“?O¥?V?X?e?€—p‚Ì—áŠO?—‚ğg—p‚µ‚Ü‚·

:option. zp
:target. any
:number. checkPacking
:usage. pack structure members with alignment {1,2,4,8,16}
:jusage. \‘¢‘Ì???“?o?[‚ğ{1,2,4,8,16}‚É?®—ñ‚µ‚Ä?p?b?N‚µ‚Ü‚·

:option. zpw
:target. any
:usage. output warning when padding is added in a class
:jusage. ?N?‰?X‚É?p?f?B?“?O‚ª’Ç‰Á‚³‚ê‚½‚Æ‚«‚ÉŒx?‚µ‚Ü‚·

:option. zq
:target. any
:usage. operate quietly (display only error messages)
:jusage. –³???b?Z?[?W?‚?[?h‚Å“®ì‚µ‚Ü‚·(?G?‰?[???b?Z?[?W‚Ì‚İ•\¦‚³‚ê‚Ü‚·)

:option. zro
:target. any
:usage. omit floating point rounding calls (non ANSI)
:jusage. omit floating point rounding calls (non ANSI)

:option. zri
:target. 386
:usage. inline floating point rounding calls
:jusage. inline floating point rounding calls

:option. zs
:target. any
:usage. syntax check only
:jusage. \•¶?`?F?b?N‚Ì‚İ‚ğs‚¢‚Ü‚·

:option. zt
:target. i86 386
:number. CmdX86CheckThreshold 256
:usage. far data threshold (i.e., larger objects go in far memory)
:jusage. far?f?[?^•~‹’l(‚Â‚Ü‚è, •~‹’l‚æ‚è‘å‚«‚¢?I?u?W?F?N?g‚ğfar???‚?Š‚É’u‚«‚Ü‚·)

:option. zu
:target. i86 386
:usage. SS != DGROUP (i.e., do not assume stack is in data segment)
:jusage. SS != DGROUP (‚Â‚Ü‚è, ?X?^?b?N‚ª?f?[?^?Z?O???“?g‚É‚ ‚é‚Æ‰¼’è‚µ‚Ü‚¹‚ñ)

:option. zv
:target. any
:usage. enable virtual function removal optimization
:jusage. ‰¼‘zŠÖ?”‚ğíœ‚·‚éÅ“K‰»‚ğs‚¢‚Ü‚·

:option. zw
:target. i86 386
:enumerate. win
:usage. generate code for Microsoft Windows
:jusage. Microsoft Windows—p‚Ì?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. zws
:target. i86
:enumerate. win
:usage. generate code for Microsoft Windows with smart callbacks
:jusage. ?X?}?[?g¥?R?[?‹?o?b?N‚ğ‚·‚éMicrosoft Windows—p?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. z\W
:target. i86
:enumerate. win
:usage. more efficient Microsoft Windows entry sequences
:jusage. ‚æ‚èŒø‰Ê“I‚ÈMicrosoft Windows?G?“?g?Š?R?[?h—ñ‚ğ?¶?¬‚µ‚Ü‚·

:option. z\Ws
:target. i86
:enumerate. win
:usage. generate code for Microsoft Windows with smart callbacks
:jusage. ?X?}?[?g¥?R?[?‹?o?b?N‚ğ‚·‚éMicrosoft Windows—p?R?[?h‚ğ?¶?¬‚µ‚Ü‚·

:option. zx
:target. i86 386
:usage. assume functions will modify FPU registers
:jusage. ŠÖ?”‚ªFPU?Œ?W?X?^‚ğ•ÏX‚·‚é‚Æ‰¼’è‚µ‚Ü‚·
:internal.

:option. zz
:target. 386
:usage. remove "@size" from __stdcall function names (10.0 compatible)
:jusage. "@size"‚ğ__stdcallŠÖ?”–¼‚©‚çíœ‚µ‚Ü‚·(10.0‚Æ‚ÌŒİŠ·?«)
