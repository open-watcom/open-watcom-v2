::
:: Make sure to make changes in ALL the sections affected
:: <97/03/26>
:segment Taxp
Usage: wccaxp [options] file [options]
:elsesegment T386
Usage: wcc386 [options] file [options]
:elsesegment
Usage: wcc [options] file [options]
:endsegment
::
Options:
:segment !OS_qnx
	    ( /option is also accepted )
:endsegment
:segment T386
-3r	      386 register calling conventions
-3s	      386 stack calling conventions
-4r	      486 register calling conventions
-4s	      486 stack calling conventions
-5r	      Pentium register calling conventions
-5s	      Pentium stack calling conventions
-6r	      Pentium Pro register calling conventions
-6s	      Pentium Pro stack calling conventions
:elsesegment Ti86
-0	      8086 instructions
-1	      80186 instructions
-2	      80286 instructions
-3	      optimize for 386
-4	      optimize for 486
-5	      optimize for Pentium
-6	      optimize for Pentium Pro
-7	      inline 80x87 instructions
:endsegment
-aa	      allow non const initializers for local aggregates or unions
-ai	      turn off type checking on static initialization
-aq	      turn off qualifier mismatch warning for const/volatile
:segment Taxp
-as	      assume short integers are aligned
:endsegment
-bd	      build target is a dynamic link library (DLL)
-bm	      build target is a multi-thread environment
:segment T386 | Taxp
-br	      build with dll run-time library
:endsegment
-bt=<id>      build target for operating system <id>
-bw	      build target is a default windowing application
-d<id>[=text] precompilation #define <id> [text]
-d0	      no debugging information
-d1{+}	      line number debugging information
-d2	      full symbolic debugging information
-d3	      full symbolic debugging with unreferenced type names
-d+	      allow extended -d macro definitions
-db	      generate browsing information
-e=<num>      set limit on number of error messages
:segment HIDDEN
-ec	      emit code coverage gear
:endsegment
:segment T386 | Ti86
-ee	      call epilogue hook routine
-ef	      use full pathnames in error and warning messages
:endsegment
-ei	      force enums to be at least as large as an int
-em	      force enum base type to use minimum integral type
:segment T386 | Ti86
-en	      emit routine names in the code segment
-ep[=<num>]   call prologue hook routine with <num> stack bytes available
:endsegment
-eq	      do not display error messages (they are still written to a file)
:segment T386
-et	      P5 profiling
-ez	      generate PharLap EZ-OMF object files
:endsegment
-fh[=<file>]  use pre-compiled headers
-fhq[=<file>] do not display pre-compiled header activity warnings
-fi=<file>    force <file> to be included
-fo[=<file>]  set object or preprocessor output file name
-fr[=<file>]  set error file name
:segment T386 | Ti86
-fp2	      generate 287 floating-point code
-fp3	      generate 387 floating-point code
-fp5	      optimize floating-point for Pentium
-fp6	      optimize floating-point for Pentium Pro
-fpc	      calls to floating-point library
-fpi	      inline 80x87 instructions with emulation
-fpi87	      inline 80x87 instructions
-fpr	      generate backward compatible 80x87 code
-g=<id>       set code group name
:endsegment
:segment T386 | Ti86
-hc	      generate Codeview debugging information
-hd	      generate DWARF debugging information
-hw	      generate Watcom debugging information
:elsesegment
-hc	      generate Codeview debugging information
-hd	      generate DWARF debugging information
:endsegment
-i=<path>     add another include path
-j	      change char default from unsigned to signed
:segment T386
-mc	      compact memory model (small code/large data)
-mf	      flat memory model (small code/small data assuming CS=DS=SS=ES)
-ml	      large memory model (large code/large data)
-mm	      medium memory model (large code/small data)
-ms	      small memory model (small code/small data)
:elsesegment Ti86
-mc	      compact memory model (small code/large data)
-mf	      flat memory model (small code/small data assuming CS=DS=SS=ES)
-mh	      huge memory model (large	code/huge data)
-ml	      large memory model (large code/large data)
-mm	      medium memory model (large code/small data)
-ms	      small memory model (small code/small data)
:endsegment
:segment T386 | Ti86
-nc=<id>      set code class name
-nd=<id>      set data segment name
:endsegment
-nm=<file>    set module name
:segment T386 | Ti86
-nt=<id>      set name of text segment
:endsegment
:segment Taxp
-o{a,b,d,e,h,i,k,l,n,o,r,s,t,u,x,z} control optimization
:elsesegment
-o{a,b,c,d,e,f[+],h,i,k,l,m,n,o,p,r,s,t,u,x,z} control optimization
:endsegment
  a	      -> relax aliasing constraints
  b	      -> branch prediction
:segment T386 | Ti86
  c	      -> disable <call followed by return> to <jump> optimization
:endsegment
  d	      -> disable all optimizations
  e[=<num>]   -> expand user functions inline (<num> controls max size)
:segment T386 | Ti86
  f	      -> generate traceable stack frames as needed
  f+	      -> always generate traceable stack frames
:endsegment
  h	      -> enable expensive optimizations
  i	      -> expand intrinsic functions inline
  k	      -> push register saves into flow graph
  l	      -> enable loop optimizations
  l+	      -> enable loop unrolling optimizations
:segment T386 | Ti86
  m	      -> generate inline code for math functions
:endsegment
  n	      -> allow numerically unstable optimizations
  o	      -> continue compilation if low on memory
  p	      -> generate consistent floating-point results
  r	      -> reorder instructions for best pipeline usage
  s	      -> favor code size over execution time in optimizations
  t	      -> favor execution time over code size in optimizations
  u	      -> all functions must have unique addresses
:segment Taxp
  x	      -> equivalent to -obiler -s
:elsesegment
  x	      -> equivalent to -obmiler -s
:endsegment
  z	      -> NULL points to valid memory in the target environment
-p{c,l,w=<num>} preprocess source file
  c	      -> preserve comments
  l	      -> insert #line directives
  w=<num>     -> wrap output lines at <num> columns. Zero means no wrap.
:segment T386 | Ti86
-r	      save/restore segment registers across calls
-ri	      return chars and shorts as ints
:endsegment
-s	      remove stack overflow checks
:segment T386 | Ti86
-sg	      generate calls to grow the stack
-st	      touch stack through SS first
:endsegment
:segment Taxp
-si	      initialize stack frame storage with pattern
:endsegment
-tp=<id>      set #pragma on( <id> )
-u<id>	      undefine macro <id>
-v	      output function declarations to .def
:segment T386 | Ti86 | Taxp
-vcap	      VC++ compatibility: alloca allowed in argument lists
:endsegment
:segment T386 & HIDDEN
-xgv	      indexed global variables
:endsegment
:segment HIDDEN
-xbsa	      emit unaligned segments
:endsegment
:segment Taxp & HIDDEN
-xd	      use default exception handler
:endsegment
-w=<num>      set warning level number
-wcd=<num>    warning control: disable warning message <num>
-wce=<num>    warning control: enable warning message <num>
-we	      treat all warnings as errors
:segment Ti86
-wo	      warn about problems with overlaid code
:endsegment
-za	      disable extensions (i.e., accept only ISO/ANSI C)
-zc	      place const data into the code segment
:segment T386 | Ti86
-zdf	      DS floats i.e. not fixed to DGROUP
-zdp	      DS is pegged to DGROUP
:endsegment
:segment T386
-zdl	      Load DS directly from DGROUP
:endsegment
-ze	      enable extensions (i.e., near, far, export, etc.)
:segment T386 | Ti86
-zff	      FS floats i.e. not fixed to a segment
-zfp	      FS is pegged to a segment
-zgf	      GS floats i.e. not fixed to a segment
-zgp	      GS is pegged to a segment
:endsegment
-zg	      generate function prototypes using base types
-zk0	      double-byte character support: Kanji
-zk0u	      translate double-byte Kanji to Unicode
-zk1	      double-byte character support: Traditional Chinese
-zk2	      double-byte character support: Korean
-zk3	      double-byte character support: Simplified Chinese
-zkl	      double-byte character support: local installed language
-zku=<num>    load UNICODE translate table for specified code page
-zl	      remove default library information
-zld	      remove file dependency information
-zm	      place each function in separate segment
-zp=<num>     pack structure members with alignment {1,2,4,8,16}
-zpw	      output warning when padding is added in a struct
:segment Taxp
-zps	      always align structs on qword boundaries
:endsegment
-zq	      operate quietly
-zs	      syntax check only
-zt[=<num>]   set far data threshold (i.e., larger objects go in far memory)
:segment T386 | Ti86
-zu	      SS != DGROUP (i.e., don't assume stack is in your data segment)
-zw	      generate code for Microsoft Windows
:endsegment
:segment Ti86
-zW	      more efficient Microsoft Windows entry sequences
:endsegment
:segment T386
-zz	      remove \"@size\" from __stdcall function names (10.0 compatible)
:endsegment
