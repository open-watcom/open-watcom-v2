/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Usage help for wcl.
*
****************************************************************************/


#ifdef WCLAXP
"Usage:  wclaxp [options] file(s)",
#elif defined(WCLPPC)
"Usage:  wclppc [options] file(s)",
#elif defined(WCLMPS)
"Usage:  wclmps [options] file(s)",
#elif defined(WCL386)
"Usage:  wcl386 [options] file(s)",
#else
"Usage:  wcl [options] file(s)",
#endif
#ifndef __UNIX__
"Options:  ( /option is also accepted )",
#endif
"-c    compile only, no link",
"-cc   treat source files as C code",
"-cc++ treat source files as C++ code",
#ifdef WCLAXP
"-y    ignore the WCLAXP environment variable",
#elif defined(WCLPPC)
"-y    ignore the WCLPPC environment variable",
#elif defined(WCLMPS)
"-y    ignore the WCLMPS environment variable",
#elif defined(WCL386)
"-y    ignore the WCL386 environment variable",
#else
"-y    ignore the WCL environment variable",
#endif
#ifdef WCL386
"[Processor options]",
"-3r 386 register calling conventions",
"-3s 386 stack calling conventions",
"-4r 486 register calling conventions",
"-4s 486 stack calling conventions",
"-5r Pentium register calling conv.",
"-5s Pentium stack calling conventions",
"-6r Pentium Pro register call conven.",
"-6s Pentium Pro stack call conven.",
#elif defined(WCLI86)
"[Processor options]",
"-0 generate code for 8086 or higher",
"-1 generate code for 186 or higher",
"-2 generate code for 286 or higher",
"-3 generate code for 386 or higher",
"-4 generate code optimized for 486",
"-5 generate code optimized for Pentium",
"-6 generate code opt. for Pentium Pro",
#endif
#if defined(WCLI86) || defined(WCL386)
"[Floating-point processor options]",
"-fpc calls to floating-point library",
"-fpd enable Pentium FDIV check",
"-fpi   inline 80x87 with emulation",
"-fpi87 inline 80x87",
"-fpr use old floating-point conventions",
"-fp2 generate 287 floating-point code",
"-fp3 generate 387 floating-point code",
"-fp5 optimize f-p for Pentium",
"-fp6 optimize f-p for Pentium Pro",
#endif
"[Compiler options]",
"-bcl=<os> compile and link for OS.",
"-bt=<os> compile for target OS.",
"-db generate browsing information",
"-e=<n> set error limit number",
#if defined(WCLI86) || defined(WCL386)
"-ecc set calling conv. to __cdecl",
"-ecd set calling conv. to __stdcall",
"-ecf set calling conv. to __fastcall",
#if 0 // not officially supported
"-eco set calling conv. to _Optlink",
#endif
"-ecp set calling conv. to __pascal",
"-ecr set calling conv. to __fortran",
"-ecs set calling conv. to __syscall",
"-ecw set calling conv. to __watcall",
#endif
"-ee call epilogue hook routine",
"-ef full paths in messages",
"-ei force enums to be type int",
"-em minimum base type for enum is int",
"-en emit routine names in the code",
"-ep[=<n>] call prologue hook routine",
"-eq do not display error messages",
#ifdef WCL386
"-et P5 profiling",
"-ez generate PharLap EZ-OMF object",
#endif
"-fh=<file> pre-compiled headers",
"-fhq[=<file>] fh without warnings",
"-fhr  (C++) only read PCH",
"-fhw  (C++) only write PCH",
"-fhwe (C++) don't count PCH warnings",
"-fi=<file> force include of file",
"-fo=<file> set object file name",
"-fr=<file> set error file name",
"-ft (C++) check for 8.3 file names",
"-fx (C++) no check for 8.3 file names",
#if defined(WCLI86) || defined(WCL386)
"-g=<codegroup> set code group name",
#endif
"-hc codeview debug format",
"-hd dwarf debug format",
"-hw watcom debug format",
"-j  change char default to signed",
#ifdef WCL386
"-m{f,s,m,c,l}  memory model",
#elif defined(WCLP16)
"-m{t,s,m,c,l,h,p}  memory model",
#elif defined(WCLI86)
"-m{t,s,m,c,l,h}  memory model",
#endif
#if defined(WCLI86) || defined(WCL386)
"-nc=<name>  set CODE class name",
"-nd=<name>  set data segment name",
"-nm=<module_name> set module name",
"-nt=<name>  set text segment name",
"-q  operate quietly",
"-r  save/restore segregs across calls",
#endif
"-ri promote function args/rets to int",
"-s  remove stack overflow checks",
#if defined(WCLI86) || defined(WCL386)
"-sg generate calls to grow the stack",
"-st touch stack through SS first",
#endif
"-v  output func declarations to .def",
"-vcap VC++ compat: alloca in arg lists",
"-w=<n>  set warning level number",
"-wcd=<n> disable warning message <n>",
"-wce=<n> enable warning message <n>",
"-we treat all warnings as errors",
"-wx set warning level to max",
#if defined(WCLI86)
"-wo diagnose problems in overlaid code",
#endif
"-xr (C++) enable RTTI",
"-z{a,e} disable/enable extensions",
"-zc place strings in CODE segment",
#if defined(WCLI86) || defined(WCL386)
"-zd{f,p} DS floats vs DS pegged to DGROUP",
#ifdef WCL386
"-zdl load DS directly from DGROUP",
#endif
"-zf{f,p} FS floats vs FS pegged to seg",
"-zg{f,p} GS floats vs GS pegged to seg",
#endif
"-zg function prototype using base type",
"-zk{0,0u,1,2,3,l} double-byte support",
"-zku=<codepage> UNICODE support",
"-zl  remove default library information",
"-zld remove file dependency information",
"-zm  place functions in separate segments",
"-zmf (C++) zm with near calls allowed",
"-zp{1,2,4,8,16} struct packing align.",
"-zpw warning when padding a struct",
"-zq operate quietly",
"-zs check syntax only",
"-zt<n> set data threshold",
#if defined(WCLI86) || defined(WCL386)
"-zu SS != DGROUP",
#endif
"-zv (C++) enable virt. fun. removal opt",
#ifdef WCL386
"-zw  generate code for MS Windows",
#endif
#ifdef WCLI86
"-zW  zw with efficient entry sequences",
"-zws -zw with smart callbacks",
"-zWs -zW with smart callbacks",
#endif
#ifdef WCL386
"-zz remove @size from __stdcall func.",
#endif
"[Debugging options]",
"-d0 no debugging information",
"-d1{+} line number debugging info.",
"-d2 full symbolic debugging info.",
"-d2i (C++) d2 + inlines as COMDATs",
"-d2s (C++) d2 + inlines as statics",
"-d2t (C++) d2 but without type names",
"-d3 debug info with unref'd type names",
"-d3i (C++) d3 + inlines as COMDATs",
"-d3s (C++) d3 + inlines as statics",
"[Optimization options]",
"-oa relax alias checking",
#if defined(WCLI86) || defined(WCL386)
"-ob branch prediction",
"-oc disable call/ret optimization",
#endif
"-od disable optimizations",
"-oe[=num] expand functions inline",
#if defined(WCLI86) || defined(WCL386)
"-of[+] generate traceable stack frames",
#endif
"-oh enable repeated optimizations",
"-oi inline intrinsic functions",
"-oi+ (C++) oi with max inlining depth",
"-ok control flow entry/exit seq.",
"-ol perform loop optimizations",
"-ol+ ol with loop unrolling",
#if defined(WCLI86) || defined(WCL386)
"-om generate inline math functions",
#endif
"-on numerically unstable floating-point",
"-oo continue compile when low on memory",
#if defined(WCLI86) || defined(WCL386)
"-op improve floating-point consistency",
#endif
"-or re-order instructions to avoid stalls",
"-os optimize for space",
"-ot optimize for time",
"-ou ensure unique addresses for functions",
#if defined(WCLAXP) || defined(WCLPPC)
"-ox maximum optimization (-oilr -s)",
#elif defined(WCLI86) || defined(WCL386)
"-ox maximum optimization (-obmiler -s)",
#endif
"[C++ exception handling options]",
"-xd  no exception handling",
"-xds no exception handling: space",
"-xdt no exception handling",
"-xs  exception handling: balanced",
"-xss exception handling: space",
"-xst exception handling: time",
"[Preprocessor options]",
"-d<name>[=text] define a macro",
"-d+ extend syntax of -d option",
"-fo=<filename> set object file name",
"-i=<directory>  include directory",
"-t=<n> (C++) # of spaces in tab stop",
"-tp=<name> (C) set #pragma on( <name>",
"-u<name>  undefine macro name",
"-pil ignore #line directives",
"-p{c,l,w=<n>} preprocess source file",
"   c     -> preserve comments",
"   l     -> insert #line directives",
"   w=<n> -> wrap output at column n",
"[Linker options]",
"-bd  build Dynamic link library",
"-bm  build Multi-thread application",
"-br  build with dll run-time library",
"-bw  build default Windowing app.",
"-bcl=<os> compile and link for OS.",
"-fd[=<file>[.lnk]] write directives",
"-fe=<executable> name executable file",
"-fm[=<map_file>]  generate map file",
"-k<stack_size> set stack size",
#ifdef WCLI86
"-lp create an OS/2 protected-mode pgm",
"-lr create a DOS real-mode program",
#endif
"-l=<os> link for the specified OS",
"-x  make names case sensitive",
"@<file> additional directive file",
"-\"<linker directives>\"",
