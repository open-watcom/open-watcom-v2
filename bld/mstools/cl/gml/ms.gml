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

:option. \1\0\x_
:target. any
:internal.
:usage. use 10.x options

:option. \C
:target. any
:usage. preserve comments

:option. \c
:target. any
:usage. compile only

:option. \D
:target. any
:special. parse_D <macro>[=<value>]
:usage. same as #define <macro>[=<value>] before compilation

:option. \E
:target. any
:usage. preprocess and insert #line directives to stdout

:option. \E\H\a
:target. any
:internal.
:usage. specify exception handling

:option. \E\H\a\c
:target. any
:internal.
:usage. specify exception handling

:option. \E\H\c
:target. any
:internal.
:usage. specify exception handling

:option. \E\H\c\a
:target. any
:internal.
:usage. specify exception handling

:option. \E\H\c\s
:target. any
:internal.
:usage. specify exception handling

:option. \E\H\s
:target. any
:internal.
:usage. specify exception handling

:option \E\H\s\c
:target. any
:internal.
:usage. specify exception handling

:option. \E\P
:target. any
:usage. preprocess without #line directives to stdout

:option. \F
:target. any
:special. parse_F <size>
:immediate. handle_F
:usage. set stack size

:option. \F\a
:target. any
:path.
:internal.
:usage. specify listing output file

:option. \F\A
:target. any
:internal.
:usage. generate assembly listing

:option. \F\A\c
:target. any
:internal.
:usage. generate assembly and machine code listing

:option. \F\A\c\s
:target. any
:internal.
:usage. generate assembly, source, and machine code listing

:option. \F\A\s
:target. any
:internal.
:usage. generate assembly and source listing

:option. \F\d
:target. any
:file.
:internal.
:usage. specify PDB filename

:option. \F\e
:target. any
:immediate. handle_Fe
:file.
:usage. set executable or DLL file name

:option. \F\I
:target. any
:special. parse_FI <file>
:usage. force <file> to be included

:option. \F\m
:target. any
:special. parse_Fm [<file>]
:usage. set map file name

:option. \F\o
:target. any
:file.
:usage. set object output file name

:option. \F\p
:target. any
:immediate. handle_Fp
:file.
:usage. set precompiled header data file name

:option. \F\R
:target. any
:immediate. handle_FR
:file.
:usage. generate browsing information

:option. \F\r
:target. any
:internal.
:usage. generate SBR file without local variables

:option. \G\3
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions

:option. \G\4
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486

:option. \G\5
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium

:option. \G\B
:target. i86
:immediate. handle_arch_i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486

:option. \G\d
:target. any
:enumerate. calling_convention
:internal.
:usage. use __cdecl (stack-based) calling convention

:option. \G\e
:target. any
:enumerate. stack_probes
:immediate. handle_stack_probes
:usage. activate stack probes for all functions

:option. \G\f
:target. any
:usage. merge duplicate strings

:option. \G\F
:target. any
:usage. merge duplicate read-only strings

:option. \G\h
:target. any
:usage. call __penter at the start of each function

:option. \G\r
:target. any
:enumerate. calling_convention
:internal.
:usage. use __fastcall (register-based) calling convention

:option. \G\s
:target. any
:enumerate. stack_probes
:immediate. handle_stack_probes
:special. parse_Gs <distance>
:usage. set stack probe distance

:option. \G\X
:target. any
:immediate. handle_GX
:usage. destruct static objects during stack unwinding

:option. \G\y
:target. any
:usage. store each function in its own COMDAT

:option. \G\z
:target. any
:enumerate. calling_convention
:internal.
:usage. use __stdcall (register-based) calling convention

:option. \H
:target. any
:number.
:internal.
:usage. set maximum identifier length

:option. help HELP ?
:target. any
:usage. get help

:option. \I
:target. any
:special. parse_I <path>
:usage. add another include path

:option. \J
:target. any
:usage. change char default from signed to unsigned

:option. \l\i\n\k
:target. any
:special. parse_link
:usage. specify linker options

:option. \L\D
:target. any
:usage. create DLL

:option. \M\D
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded DLL version of C library

:option. \M\D\d
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded debug DLL version of C library

:option. \M\L
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use single-thread statically linked version of C library

:option. \M\L\d
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use single-thread debug static link version of C library

:option. \M\T
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded static version of C library

:option. \M\T\d
:target. any
:immediate. handle_threads_linking
:enumerate. threads_linking
:usage. use multithreaded debug static version of C library

:option. \n\o\l\o\g\o
:target. any
:usage. operate quietly

:option. \O\1
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. minimize size

:option. \O\2
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. maximize speed

:option. \O\a
:target. any
:usage. assume no aliasing

:option. \O\b
:target. any
:immediate. handle_inlining_level
:number. check_inlining_level
:usage. control function inlining

:option. \O\d
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. disable all optimizations

:option. \O\g
:target. any
:usage. enable global optimizations

:option. \O\i
:target. any
:usage. expand intrinsic functions inline

:option. \O\p
:target. any
:immediate. handle_Op
:timestamp.
:usage. generate consistent floating-point results

:option. \O\s
:target. any
:immediate. handle_opt_size_time
:enumerate. opt_size_time
:usage. favor code size over execution time in optimizations

:option. \O\t
:target. any
:immediate. handle_opt_size_time
:enumerate. opt_size_time
:usage. favor execution time over code size in optimizations

:option. \O\w
:target. any
:internal.
:usage. assume aliasing across function calls

:option. \O\x
:target. any
:immediate. handle_opt_level
:enumerate. opt_level
:usage. equivalent to /Ob1 /Og /Oi /Ot /Oy /Gs

:option. \O\y
:target. any
:immediate. handle_Oy
:usage. disable stack frames

:option. \o
:target. any
:special. parse_o <file>
:usage. set executable or DLL file name

:option. \P
:target. any
:usage. preprocess to a file

:option. \Q\I\f\d\i\v
:target. i86
:immediate. handle_QIfdiv
:usage. enable Pentium FDIV fix

:option. \s\h\o\w\w\o\p\t\s
:target. any
:usage. show translated options

:option. \p\a\s\s\w\o\p\t\s
:target. any
:special. parse_passwopts :<options>
:usage. pass <options> directly to the Watcom tools

:option. \n\o\i\n\v\o\k\e
:target. any
:usage. don't invoke the Watcom tool

:option. \n\o\w\o\p\t\s
:target. any
:usage. disable default options

:option. \n\o\w\w\a\r\n
:target. any
:immediate. handle_nowwarn
:usage. disable warning messages for ignored options

:option. \l\e\s\s\w\d
:target. any
:internal.
:usage. change debug info from -d2 to -d1

:option. \T\C
:target. any
:immediate. handle_TC
:usage. force compilation of all files as C

:option. \T\c
:target. any
:special. parse_Tc <file>
:usage. force compilation of <file> as C

:option. \T\P
:target. any
:immediate. handle_TP
:usage. force compilation of all files as C++

:option. \T\p
:target. any
:special. parse_Tp <file>
:usage. force compilation of <file> as C++

:option. \U
:target. any
:special. parse_U <macro>
:usage. undefine macro name

:option. \u
:target. any
:internal.
:usage. undefine all predefined macros

:option. \v\d\0
:target. any
:internal.
:usage. disable constructor/destructor displacements

:option. \v\d\1
:target. any
:internal.
:usage. enable vtordisp constructor/destructor displacements

:option. \v\m\b
:target. any
:internal.
:usage. use best case always pointer representation

:option. \v\m\g
:target. any
:internal.
:usage. use general purpose always pointer representation

:option. \v\m\m
:target. any
:internal.
:usage. general-purpose pointers to single- and multiple-inheritance classes

:option. \v\m\s
:target. any
:internal.
:usage. general-purpose pointers to single-inheritance classes

:option. \v\m\v
:target. any
:internal.
:usage. general-purpose pointers to any classes

:option. \V
:target. any
:special. parse_V
:internal.
:usage. embed string in object file

:option. \W
:target. any
:immediate. handle_warn_level
:enumerate. warn_level
:number. check_warn_level
:usage. set warning level number

:option. \w
:target. any
:immediate. handle_warn_level
:enumerate. warn_level
:usage. disable all warning messages

:option. \W\X
:target. any
:usage. treat all warnings as errors

:option. \X
:target. any
:internal.
:usage. ignore standard include paths

:option. \Y\c
:target. any
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. create pre-compiled header file

:option. \Y\d
:target. any
:usage. full debug info from pre-compiled headers

:option. \Y\u
:target. any
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. use pre-compiled header file

:option. \Y\X
:target. any
:enumerate. precomp_headers
:immediate. handle_precomp_headers
:file.
:optional.
:usage. use pre-compiled header file

:option. \Z\7
:target. any
:immediate. handle_debug_info
:enumerate. debug_info
:usage. generate Codeview debugging information

:option. \Z\a
:target. any
:enumerate. iso
:timestamp.
:usage. disable extensions (i.e., accept only ISO/ANSI C++)

:option. \Z\d
:target. any
:immediate. handle_debug_info
:enumerate. debug_info
:usage. line number debugging information

:option. \Z\e
:target. any
:enumerate. iso
:usage. enable extensions (e.g., near, far, export, etc.)

:option. \Z\g
:target. any
:usage. output function declarations to stdout

:option. \Z\i
:target. any
:immediate. handle_debug_info
:enumerate. debug_info
:internal.
:usage. full symbolic debugging information

:option. \Z\l
:target. any
:usage. remove default library information

:option. \Z\m
:target. any
:number. check_maxmem
:usage. maximum memory allocation in % of default (ignored)

:option. \Z\n
:target. any
:internal.
:usage. disable SBR file packing

:option. \Z\p
:target. any
:number. check_packing 1
:usage. pack structure members with alignment {1,2,4,8,16}

:option. \Z\s
:target. any
:usage. syntax check only
