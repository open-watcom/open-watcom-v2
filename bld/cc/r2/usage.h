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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


"Usage: wcc386 [options] file [options]",
"Options:",
"            ( /option is also accepted )",
"-b{m,d,w}      build (Multi thread,Dynamic link,Default windowing)",
"-bt[=<os>]     build target for operating system <os>",
"-db            generate browsing information",
"-d1{+}         line number debugging information",
"-d2            full symbolic debugging information",
"-d3            full symbolic debugging with unreferenced type names",
"-d+            allow extended -d macro definitions",
"-d<name>[=text] precompilation #define name [text]",
"-e<number>     set error limit number",
"-fh=<file_name> use pre-compiled headers",
"-fhq=<file_name> use pre-compiled headers quietly",
"-fi=<file_name> force file_name to be included",
"-fo=<file_name> set object or preprocessor output file name",
"-fr=<file_name> set error file name",
"-ei            force enums to be type int",
"-en            emit routine names in the code segment",
"-ep[<number>]  call prologue hook routine with number stack bytes available",
"-ee            call epilogue hook routine",
"-ez            generate PharLap EZ-OMF object files",
"-fpc           calls to floating-point library",
"-fpi           inline 80x87 instructions with emulation",
"-fpi87         inline 80x87 instructions",
"-fp2           optimize floating-point for 287",
"-fp3           optimize floating-point for 387",
"-fp5           optimize floating-point for Pentium",
"-fpr           generate backward compatible 80x87 code",
"-g=<codegroup> set code group name",
"-h{w,d,c}      set debug output format (Watcom,Dwarf,Codeview)",
"-i=<directory> another include directory",
"-j             change char default from unsigned to signed",
"-m{s,m,c,l,f}  memory model (Small,Medium,Compact,Large,Flat)",
"-3r            386 register calling conventions",
"-3s            386 stack calling conventions",
"-4r            486 register calling conventions",
"-4s            486 stack calling conventions",
"-5r            Pentium register calling conventions",
"-5s            Pentium stack calling conventions",
"-nc=<name>     set code class name",
"-nd=<name>     set data segment name",
"-nm=<name>     set module name",
"-nt=<name>     set name of text segment",
"-o{a,c,d,e,f[+],i,l,m,n,o,p,r,s,t,u,x} control optimization",
"    a          -> relax aliasing constraints",
"    c          -> disable <call followed by return> to <jump> optimization",
"    d          -> disable all optimizations",
"    e[=<num>]  -> expand user functions inline. <num> controls max size",
"    f          -> generate traceable stack frames as needed",
"    f+         -> always generate traceable stack frames",
"    i          -> expand intrinsic functions inline",
"    l          -> enable loop optimizations",
"    l+         -> enable loop unrolling",
"    m          -> generate inline 80x87 code for math functions",
"    n          -> allow numerically unstable optimizations",
"    o          -> continue compilation if low on memory",
"    p          -> generate consistent floating-point results",
"    r          -> reorder instructions for best pipeline usage",
"    s          -> favor code size over execution time in optimizations",
"    t          -> favor execution time over code size in optimizations",
"    u          -> all functions must have unique addresses",
"    x          -> equivalent to -omiler -s",
"    z          -> do not assume a pointer deref implies pointer not NULL",
"-p{l,c,w=<num>} preprocess file",
"    l          -> insert #line directives",
"    c          -> preserve comments",
"    w=<num>    -> wrap output lines at <num> columns. Zero means no wrap.",
"-r             save/restore segment registers across calls",
"-ri            return chars and shorts as ints",
"-s             remove stack overflow checks",
"-sg            generate calls to grow the stack",
"-st            touch stack through SS first",
"-u<name>       undefine macro name",
"-v             output function declarations to .def",
"-w<number>     set warning level number",
"-we            treat all warnings as errors",
"-z{a,e}        disable/enable language extensions",
"-zc            place literal strings in the code segment",
"-zdf           DS floats i.e. not fixed to DGROUP",
"-zdp           DS is pegged to DGROUP",
"-zdl           Load DS directly from DGROUP",
"-zff           FS floats i.e. not fixed to a segment",
"-zfp           FS is pegged to a segment",
"-zgf           GS floats i.e. not fixed to a segment",
"-zgp           GS is pegged to a segment",
"-zg            generate function prototypes using base types",
"-zk{0,1,2}     double-byte char support: 0=Kanji,1=Chinese/Taiwanese,2=Korean",
"-zk0u          translate double-byte Kanji to UNICODE",
"-zku=<codepage> load UNICODE translate table for specified code page",
"-zl            remove default library information",
"-zld           remove file dependency information",
"-zm            place each function in separate segment",
"-zp{1,2,4,8}   pack structure members",
"-zq            operate quietly",
"-zs            syntax check only",
"-zt<number>    set data threshold",
"-zu            SS != DGROUP",
"-zw            generate code for Microsoft Windows",
