:: *************************************************************************
::
:: Description:  Usage help for owcc.
::
:: *************************************************************************
Usage:  owcc [options] file ...

-c                              compile only, no link
-x {c,c++}                      treat source files as C/C++ code
-o <name>                       set output file name
-b <target>                     compile and link for target
-v                              show sub program invocations


                [Processor options]
:: FIXME: what about non-i86 CPUs?
-mtune={3,4,5,6}86              optimize for this family
-mregparm=<n>                   calling convention


                [x86 Floating-point processor options]
-fpmath={2,3}87                 hardware floating-point code
-fptune={5,6}86                 optimize f-p for CPU type
-msoft-float                    calls to floating-point library


                [Compiler options]
-mcmodel={f,t,s,m,c,l,h}        memory model
-mabi=c                         set calling conv. to __cdecl
-mabi=d                         set calling conv. to __stdcall
-mabi=f                         set calling conv. to __fastcall
:: _Optlink is undocumented
:: -mabi=o                         set calling conv. to _Optlink
-mabi=p                         set calling conv. to __pascal
-mabi=r                         set calling conv. to __fortran
-mabi=s                         set calling conv. to _System
-mabi=w                         set calling conv. to __watcall
-mwindows                       generate code for MS Windows
-shared                         generate code for a DLL
-fno-short-enum                 force enums to be type int
-fshort-enum                    use minimum base type for enum
-fsigned-char                   change char default to signed
-fpack-struct={1,2,4,8,16}      default struct member alignment
-fno-rtti                       (C++) disable RTTI
-std={c89,c99,wc}               disable/enable extensions

-Wlevel<n>                      set warning level number
-Wall                           set warning level to 4
-Wextra                         enable maximal warnings
-Werror                         treat all warnings as errors
-Wn<n>                          enable warning message <n>
-Wno-n<n>                       disable warning message <n>
-Woverlay                       warn about problems in overlaying
-Wpadded                        warn when padding a struct
-Wstop-after-errors=<n>         stop compiler after <n> errors
-fmessage-full-path             full paths in messages
-femit-names                    emit routine names in the code
-fbrowser                       generate browsing information

-fhook-epilogue                 call epilogue hook routine
-fhook-prologue[=<n>]           call prologue hook routine
-fo=<ext>                       set object file extension
-fr=<file>                      output errors to a file name
-fwrite-def                     output func declarations to .def
-fwrite-def-without-typedefs    same, but skip typedef names
-fno-stack-check                remove stack overflow checks
-fgrow-stack                    generate calls to grow the stack
-fstack-probe                   touch stack through SS first
-fno-writable-strings           don't place strings in CODE segment
-fnostdlib                      remove default library information
-ffunction-sections             functions in separate segments
-fvoid-ptr-arithmetic           allow arithmetic operations on (void *)

-fsyntax-only                   check syntax only
-Wc,<option>                    pass any option to WCC


                [Debugging options]
-g0                             no debugging information
-g1{+}                          line number debugging info.
-g2                             full symbolic debugging info.
-g2i                            (C++) + inlines as COMDATs
-g2s                            (C++) + inlines as statics
-g2t                            (C++) without type names
-g3                             add info for unreferenced type names
-g3i                            (C++) + inlines as COMDATs
-g3s                            (C++) + inlines as statics
:: FIXME: check actual behaviour!
-gcodeview                      codeview debug format
-gdwarf                         dwarf debug format
-gwatcom                        watcom debug format


                [Optimization options]
-O0                             disable optimizations
-O{1,2,3}                       optimization presets
-Os                             optimize for space
-Ot                             optimize for time

-fno-strict-aliasing            relax alias checking
-fguess-branch-probability      branch prediction
-fno-optimize-sibling-calls     disable call/ret optimization
-finline-functions              expand functions inline
-finline-limit=num              which functions to expand inline
-fno-omit-frame-pointer         generate traceable stack frames
-fno-omit-leaf-frame-pointer    generate more stack frames
-frerun-optimizer               enable repeated optimizations
-finline-intrinsics[-max]       inline intrinsic functions
-fschedule-prologue             control flow entry/exit seq.
-floop-optimize                 perform loop optimizations
-funroll-loops                  perform loop unrolling
-finline-math                   generate inline math functions
-funsafe-math-optimizations     numerically unstable floating-point
-ffloat-store                   improve floating-point consistency
-fschedule-insns                re-order instructions to avoid stalls
-fkeep-duplicates               ensure unique addresses for functions

-feh[=s,=t]                     exception handling: balanced, space, time


                [Preprocessor options]
-D<name>[=text]                 define a macro
-D+                             extend syntax of -D option
-U<name>                        undefine macro name
-I <directory>                  include directory
-include <file>                 force pre-include of file
-ftabstob=<n>                   # of spaces in tab stop
-tp=<name>                      (C) set #pragma on <name>
:: FIXME: rename!
-pil                            ignore #line directives
-E                              preprocess source file
-C                              preserve comments
-P                              don't insert #line directives
-fcpp-wrap=<n>                  wrap output at column n
-fmangle-cpp                    encrypt C++ names
-M[M]D                          output autodepend make rule
-MF <file>                      autodepend output name
-MT <target>                    autodepend target name


                [Linker options]
-mthreads                       build Multi-thread application
-mrtdll                         build with dll run-time library
-fd[=<file>]                    save WLINK directives to file
-fm[=<map_file>]                generate map file
-mstack-size=<stack_size>       set stack size
-s                              strip debug info from executable
-Wl,<option>                    pass any directive to WLINK
