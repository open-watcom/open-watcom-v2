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


#include "ftnstd.h"

static const char __FAR Msg11008[] = {
 0
,0
/* generate a listing file  */
,4,2,13,18,0
/* listing file to printer  */
,4,18,0,4,169
/* listing file to terminal  */
,4,18,0,4,183
/* listing file to disk  */
,4,18,0,4,127
/* generate an error file  */
,3,2,112,0
/* issue extension messages  */
,3,32,133,21
/* issue unreferenced warning  */
,3,32,186,64
/* issue warning messages  */
,3,32,64,21
/* INTEGER/LOGICAL size 2/1  */
,3,97,63,79
/* list INCLUDE files  */
,2,150,41
/* display diagnostic messages  */
,2,128,21
/* explicit typing required  */
,1,132
/* extend line length  */
,3,50,55,147
,0
/* /trace /bounds  */
,1,78
/* define macro  */
,1,121
/* relax FORMAT type checking  */
,3,62,94,16
/* relax wild branch checking  */
,4,62,187,44,16
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* syntax check only  */
,3,181,45,160
/* generate default libraries  */
,3,2,30,148
/* floating-point calls  */
,1,135
/* emulate 80x87 instructions  */
,3,129,28,17
/* in-line 80x87 instructions  */
,3,54,28,17
/* 80287 instructions  */
,2,82,17
/* 80387 instructions  */
,2,84,17
/* /fp3 optimized for Pentium  */
,4,38,11,1,8
/* 80x87 reverse compatibility  */
,2,28,173
/* line # debugging information  */
,4,55,65,9,10
/* full debugging information  */
,3,137,9,10
,0
/* no call-ret optimizations  */
,3,35,115,3
/* disable optimizations  */
,2,126,3
/* no stack frame optimizations  */
,4,35,25,136,3
/* statement functions in-line  */
,2,180,54
/* loop optimizations  */
,2,19,3
/* math optimizations  */
,2,153,3
/* numerical optimizations  */
,2,157,3
/* precision optimizations  */
,2,61,3
/* instruction scheduling  */
,1,143
/* space optimizations  */
,2,178,3
/* time optimizations  */
,2,185,3
,0
/* /o[bp,i,l,k,m,r,t,do]  */
,1,77
,0
/* stack checking  */
,2,25,16
/* set data threshold  */
,3,12,48,184
/* local variables on the stack  */
,4,57,37,159,25
/* pass character descriptors  */
,3,166,15,123
/* SAVE local variables  */
,3,104,57,37
/* name object file  */
,3,58,158,0
/* constants in code segment  */
,4,120,53,46,24
/* medium memory model  */
,3,154,20,34
/* large memory model  */
,3,146,20,34
/* huge memory model  */
,3,138,20,34
/* assume 8086 processor  */
,3,14,86,23
/* assume 80186 processor  */
,3,14,80,23
/* assume 80286 processor  */
,3,14,81,23
,0
,0
/* default windowed application  */
,2,30,188
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* assume 80386 processor  */
,3,14,83,23
/* /3 optimized for 80486  */
,4,27,11,1,85
/* /3 optimized for Pentium  */
,4,27,11,1,8
/* save/restore segment regs  */
,3,176,24,171
/* FS not fixed  */
,3,95,22,51
/* GS not fixed  */
,3,96,22,51
/* SS not default data segment  */
,5,105,22,30,48,24
/* generate code for Windows  */
,4,2,46,1,108
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* *File Management Options*  */
,2,72,6
/* *Diagnostic Options*  */
,2,71,6
/* *Debugging Options*  */
,2,70,6
/* *Miscellaneous Options*  */
,2,75,6
/* *Floating-Point Options*  */
,2,73,6
/* *Optimizations*  */
,1,76
/* *Memory Models*  */
,1,74
/* *CPU Targets*  */
,1,67
/* *Compile and Link Options*  */
,2,69,6
/* Usage: wfl {- or /<option> | <file>}  */
,1,106
,0
/* compile only, no linking  */
,3,117,35,149
/* ignore WFL variable  */
,1,139
/* run protected mode compiler  */
,2,174,47
/* generate map file  */
,3,2,152,0
/* name executable file  */
,3,58,131,0
/* keep directive file  */
,3,145,49,0
/* include directive file  */
,3,140,49,0
/* link for OS/2 protect-mode  */
,3,33,1,102
/* link for DOS real-mode  */
,3,33,1,91
/* link for specified system  */
,3,33,1,179
/* set stack size  */
,3,12,25,63
/* additional linker directives  */
,3,109,56,125
/* Out of memory  */
,3,103,59,20
/* Error: Unable to open temporary file  */
,6,5,7,4,60,182,0
/* Unable to open '%s1'  */
,4,7,4,60,26
/* Error: Unable to invoke compiler  */
,5,5,7,4,31,47
/* Error: Compiler returned a bad status compiling '%s1'  */
,7,5,89,36,13,29,119,26
/* Error: Unable to invoke linker  */
,5,5,7,4,31,56
/* Error: Linker returned a bad status  */
,5,5,101,36,13,29
/* Error: Unable to find '%s1'  */
,5,5,7,4,134,26
/* generate run-time traceback  */
,2,2,175
/* generate bounds checking  */
,3,2,113,16
/* *Character Set Options*  */
,2,68,6
/* Chinese character set  */
,3,87,15,12
/* Japanese character set  */
,3,98,15,12
/* Korean character set  */
,3,99,15,12
/* generate file dependencies  */
,3,2,0,122
/* *Application Type*  */
,1,66
/* messages in resource file  */
,4,21,53,172,0
/* devices are carriage control  */
,3,124,43,116
/* operate quietly  */
,1,161
,0
,0
,0
,0
,0
/* generate browse information  */
,3,2,114,10
/* extend float-pt. precision  */
,3,50,52,61
/* loop optimizations/unrolling  */
,2,19,163
/* align COMMON segments  */
,3,110,39,177
/* LF with FF  */
,1,100
/* WATCOM debugging information  */
,3,107,9,10
/* DWARF debugging information  */
,3,92,9,10
/* Codeview debugging info.  */
,3,88,9,141
/* Error: Unable to invoke CVPACK  */
,5,5,7,4,31,40
/* Error: CVPACK returned a bad status  */
,5,5,40,36,13,29
/* DO-variables do not overflow  */
,3,90,22,165
/* path for INCLUDE files  */
,3,167,1,41
/* enable Pentium FDIV check  */
,4,130,8,93,45
/* branch prediction  */
,2,44,168
/* mangle COMMON segment names  */
,4,151,39,24,156
/* promote intrinsic arguments  */
,1,170
/* allow comma separator  */
,1,111
/* /fp3 optimized for Pentium Pro  */
,5,38,11,1,8,42
/* /3 optimized for Pentium Pro  */
,5,27,11,1,8,42
/* optimize at expense of compile-time  */
,3,164,59,118
/* move register saves into flow graph  */
,1,155
/* loop optimizations/assume loop invariant float-pt. variables are initialized  */
,8,19,162,19,144,52,37,43,142
};


extern const char __FAR * const __FAR GroupTable[] = {
     NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,Msg11008
    };


extern const char __FAR ErrWord[] = {
          4,'f','i','l','e'
/* count= 13 Group:offset = MS: 2, MS: 3, MS: 4, MS: 5, MS: 6,... */
          ,3,'f','o','r'
/* count= 10 Group:offset = MS:43, MS:89, MS:90, MS:95, MS:179,... */
          ,8,'g','e','n','e','r','a','t','e'
/* count=  9 Group:offset = MS: 2, MS: 6, MS:37, MS:95, MS:175,... */
          ,13,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s'
/* count=  9 Group:offset = MS:48, MS:49, MS:50, MS:52, MS:53,... */
          ,2,'t','o'
/* count=  9 Group:offset = MS: 3, MS: 4, MS: 5, MS:185, MS:186,... */
          ,6,'E','r','r','o','r',':'
/* count=  8 Group:offset = MS:185, MS:187, MS:188, MS:189, MS:190,... */
          ,8,'O','p','t','i','o','n','s','*'
/* count=  7 Group:offset = MS:161, MS:162, MS:163, MS:164, MS:165,... */
          ,6,'U','n','a','b','l','e'
/* count=  6 Group:offset = MS:185, MS:186, MS:187, MS:189, MS:191,... */
          ,7,'P','e','n','t','i','u','m'
/* count=  5 Group:offset = MS:43, MS:90, MS:220, MS:225, MS:226 */
          ,9,'d','e','b','u','g','g','i','n','g'
/* count=  5 Group:offset = MS:45, MS:46, MS:213, MS:214, MS:215 */
          ,11,'i','n','f','o','r','m','a','t','i'
              ,'o','n'
/* count=  5 Group:offset = MS:45, MS:46, MS:208, MS:213, MS:214 */
          ,9,'o','p','t','i','m','i','z','e','d'
/* count=  5 Group:offset = MS:43, MS:89, MS:90, MS:225, MS:226 */
          ,3,'s','e','t'
/* count=  5 Group:offset = MS:63, MS:182, MS:195, MS:196, MS:197 */
          ,1,'a'
/* count=  4 Group:offset = MS: 2, MS:188, MS:190, MS:217 */
          ,6,'a','s','s','u','m','e'
/* count=  4 Group:offset = MS:72, MS:73, MS:74, MS:88 */
          ,9,'c','h','a','r','a','c','t','e','r'
/* count=  4 Group:offset = MS:65, MS:195, MS:196, MS:197 */
          ,8,'c','h','e','c','k','i','n','g'
/* count=  4 Group:offset = MS:18, MS:19, MS:62, MS:193 */
          ,12,'i','n','s','t','r','u','c','t','i'
              ,'o','n','s'
/* count=  4 Group:offset = MS:39, MS:40, MS:41, MS:42 */
          ,7,'l','i','s','t','i','n','g'
/* count=  4 Group:offset = MS: 2, MS: 3, MS: 4, MS: 5 */
          ,4,'l','o','o','p'
/* count=  4 Group:offset = MS:52, MS:210, MS:229 */
          ,6,'m','e','m','o','r','y'
/* count=  4 Group:offset = MS:69, MS:70, MS:71, MS:184 */
          ,8,'m','e','s','s','a','g','e','s'
/* count=  4 Group:offset = MS: 7, MS: 9, MS:12, MS:200 */
          ,3,'n','o','t'
/* count=  4 Group:offset = MS:92, MS:93, MS:94, MS:218 */
          ,9,'p','r','o','c','e','s','s','o','r'
/* count=  4 Group:offset = MS:72, MS:73, MS:74, MS:88 */
          ,7,'s','e','g','m','e','n','t'
/* count=  4 Group:offset = MS:68, MS:91, MS:94, MS:222 */
          ,5,'s','t','a','c','k'
/* count=  4 Group:offset = MS:50, MS:62, MS:64, MS:182 */
          ,5,'\'','%','s','1','\''
/* count=  3 Group:offset = MS:186, MS:188, MS:191 */
          ,2,'/','3'
/* count=  3 Group:offset = MS:89, MS:90, MS:226 */
          ,5,'8','0','x','8','7'
/* count=  3 Group:offset = MS:39, MS:40, MS:44 */
          ,10,'b','a','d',' ','s','t','a','t','u'
              ,'s'
/* count=  3 Group:offset = MS:188, MS:190, MS:217 */
          ,7,'d','e','f','a','u','l','t'
/* count=  3 Group:offset = MS:37, MS:77, MS:94 */
          ,6,'i','n','v','o','k','e'
/* count=  3 Group:offset = MS:187, MS:189, MS:216 */
          ,5,'i','s','s','u','e'
/* count=  3 Group:offset = MS: 7, MS: 8, MS: 9 */
          ,4,'l','i','n','k'
/* count=  3 Group:offset = MS:179, MS:180, MS:181 */
          ,5,'m','o','d','e','l'
/* count=  3 Group:offset = MS:69, MS:70, MS:71 */
          ,2,'n','o'
/* count=  3 Group:offset = MS:48, MS:50, MS:172 */
          ,8,'r','e','t','u','r','n','e','d'
/* count=  3 Group:offset = MS:188, MS:190, MS:217 */
          ,9,'v','a','r','i','a','b','l','e','s'
/* count=  3 Group:offset = MS:64, MS:66, MS:229 */
          ,4,'/','f','p','3'
/* count=  2 Group:offset = MS:43, MS:225 */
          ,6,'C','O','M','M','O','N'
/* count=  2 Group:offset = MS:211, MS:222 */
          ,6,'C','V','P','A','C','K'
/* count=  2 Group:offset = MS:216, MS:217 */
          ,13,'I','N','C','L','U','D','E',' ','f'
              ,'i','l','e','s'
/* count=  2 Group:offset = MS:11, MS:219 */
          ,3,'P','r','o'
/* count=  2 Group:offset = MS:225, MS:226 */
          ,3,'a','r','e'
/* count=  2 Group:offset = MS:201, MS:229 */
          ,6,'b','r','a','n','c','h'
/* count=  2 Group:offset = MS:19, MS:221 */
          ,5,'c','h','e','c','k'
/* count=  2 Group:offset = MS:36, MS:220 */
          ,4,'c','o','d','e'
/* count=  2 Group:offset = MS:68, MS:95 */
          ,8,'c','o','m','p','i','l','e','r'
/* count=  2 Group:offset = MS:174, MS:187 */
          ,4,'d','a','t','a'
/* count=  2 Group:offset = MS:63, MS:94 */
          ,9,'d','i','r','e','c','t','i','v','e'
/* count=  2 Group:offset = MS:177, MS:178 */
          ,6,'e','x','t','e','n','d'
/* count=  2 Group:offset = MS:14, MS:209 */
          ,5,'f','i','x','e','d'
/* count=  2 Group:offset = MS:92, MS:93 */
          ,9,'f','l','o','a','t','-','p','t','.'
/* count=  2 Group:offset = MS:209, MS:229 */
          ,2,'i','n'
/* count=  2 Group:offset = MS:68, MS:200 */
          ,7,'i','n','-','l','i','n','e'
/* count=  2 Group:offset = MS:40, MS:51 */
          ,4,'l','i','n','e'
/* count=  2 Group:offset = MS:14, MS:45 */
          ,6,'l','i','n','k','e','r'
/* count=  2 Group:offset = MS:183, MS:189 */
          ,5,'l','o','c','a','l'
/* count=  2 Group:offset = MS:64, MS:66 */
          ,4,'n','a','m','e'
/* count=  2 Group:offset = MS:67, MS:176 */
          ,2,'o','f'
/* count=  2 Group:offset = MS:184, MS:227 */
          ,4,'o','p','e','n'
/* count=  2 Group:offset = MS:185, MS:186 */
          ,9,'p','r','e','c','i','s','i','o','n'
/* count=  2 Group:offset = MS:55, MS:209 */
          ,5,'r','e','l','a','x'
/* count=  2 Group:offset = MS:18, MS:19 */
          ,4,'s','i','z','e'
/* count=  2 Group:offset = MS:10, MS:182 */
          ,7,'w','a','r','n','i','n','g'
/* count=  2 Group:offset = MS: 8, MS: 9 */
          ,1,'#'
/* count=  1 Group:offset = MS:45 */
          ,18,'*','A','p','p','l','i','c','a','t'
              ,'i','o','n',' ','T','y','p','e','*'
/* count=  1 Group:offset = MS:199 */
          ,13,'*','C','P','U',' ','T','a','r','g'
              ,'e','t','s','*'
/* count=  1 Group:offset = MS:168 */
          ,14,'*','C','h','a','r','a','c','t','e'
              ,'r',' ','S','e','t'
/* count=  1 Group:offset = MS:194 */
          ,17,'*','C','o','m','p','i','l','e',' '
              ,'a','n','d',' ','L','i','n','k'
/* count=  1 Group:offset = MS:169 */
          ,10,'*','D','e','b','u','g','g','i','n'
              ,'g'
/* count=  1 Group:offset = MS:163 */
          ,11,'*','D','i','a','g','n','o','s','t'
              ,'i','c'
/* count=  1 Group:offset = MS:162 */
          ,16,'*','F','i','l','e',' ','M','a','n'
              ,'a','g','e','m','e','n','t'
/* count=  1 Group:offset = MS:161 */
          ,15,'*','F','l','o','a','t','i','n','g'
              ,'-','P','o','i','n','t'
/* count=  1 Group:offset = MS:165 */
          ,15,'*','M','e','m','o','r','y',' ','M'
              ,'o','d','e','l','s','*'
/* count=  1 Group:offset = MS:167 */
          ,14,'*','M','i','s','c','e','l','l','a'
              ,'n','e','o','u','s'
/* count=  1 Group:offset = MS:164 */
          ,15,'*','O','p','t','i','m','i','z','a'
              ,'t','i','o','n','s','*'
/* count=  1 Group:offset = MS:166 */
          ,21,'/','o','[','b','p',',','i',',','l'
              ,',','k',',','m',',','r',',','t',',','d'
              ,'o',']'
/* count=  1 Group:offset = MS:60 */
          ,14,'/','t','r','a','c','e',' ','/','b'
              ,'o','u','n','d','s'
/* count=  1 Group:offset = MS:16 */
          ,3,'2','/','1'
/* count=  1 Group:offset = MS:10 */
          ,5,'8','0','1','8','6'
/* count=  1 Group:offset = MS:73 */
          ,5,'8','0','2','8','6'
/* count=  1 Group:offset = MS:74 */
          ,5,'8','0','2','8','7'
/* count=  1 Group:offset = MS:41 */
          ,5,'8','0','3','8','6'
/* count=  1 Group:offset = MS:88 */
          ,5,'8','0','3','8','7'
/* count=  1 Group:offset = MS:42 */
          ,5,'8','0','4','8','6'
/* count=  1 Group:offset = MS:89 */
          ,4,'8','0','8','6'
/* count=  1 Group:offset = MS:72 */
          ,7,'C','h','i','n','e','s','e'
/* count=  1 Group:offset = MS:195 */
          ,8,'C','o','d','e','v','i','e','w'
/* count=  1 Group:offset = MS:215 */
          ,8,'C','o','m','p','i','l','e','r'
/* count=  1 Group:offset = MS:188 */
          ,15,'D','O','-','v','a','r','i','a','b'
              ,'l','e','s',' ','d','o'
/* count=  1 Group:offset = MS:218 */
          ,13,'D','O','S',' ','r','e','a','l','-'
              ,'m','o','d','e'
/* count=  1 Group:offset = MS:180 */
          ,5,'D','W','A','R','F'
/* count=  1 Group:offset = MS:214 */
          ,4,'F','D','I','V'
/* count=  1 Group:offset = MS:220 */
          ,11,'F','O','R','M','A','T',' ','t','y'
              ,'p','e'
/* count=  1 Group:offset = MS:18 */
          ,2,'F','S'
/* count=  1 Group:offset = MS:92 */
          ,2,'G','S'
/* count=  1 Group:offset = MS:93 */
          ,15,'I','N','T','E','G','E','R','/','L'
              ,'O','G','I','C','A','L'
/* count=  1 Group:offset = MS:10 */
          ,8,'J','a','p','a','n','e','s','e'
/* count=  1 Group:offset = MS:196 */
          ,6,'K','o','r','e','a','n'
/* count=  1 Group:offset = MS:197 */
          ,10,'L','F',' ','w','i','t','h',' ','F'
              ,'F'
/* count=  1 Group:offset = MS:212 */
          ,6,'L','i','n','k','e','r'
/* count=  1 Group:offset = MS:190 */
          ,17,'O','S','/','2',' ','p','r','o','t'
              ,'e','c','t','-','m','o','d','e'
/* count=  1 Group:offset = MS:179 */
          ,3,'O','u','t'
/* count=  1 Group:offset = MS:184 */
          ,4,'S','A','V','E'
/* count=  1 Group:offset = MS:66 */
          ,2,'S','S'
/* count=  1 Group:offset = MS:94 */
          ,36,'U','s','a','g','e',':',' ','w','f'
              ,'l',' ','{','-',' ','o','r',' ','/','<'
              ,'o','p','t','i','o','n','>',' ','|',' '
              ,'<','f','i','l','e','>','}'
/* count=  1 Group:offset = MS:170 */
          ,6,'W','A','T','C','O','M'
/* count=  1 Group:offset = MS:213 */
          ,7,'W','i','n','d','o','w','s'
/* count=  1 Group:offset = MS:95 */
          ,10,'a','d','d','i','t','i','o','n','a'
              ,'l'
/* count=  1 Group:offset = MS:183 */
          ,5,'a','l','i','g','n'
/* count=  1 Group:offset = MS:211 */
          ,21,'a','l','l','o','w',' ','c','o','m'
              ,'m','a',' ','s','e','p','a','r','a','t'
              ,'o','r'
/* count=  1 Group:offset = MS:224 */
          ,8,'a','n',' ','e','r','r','o','r'
/* count=  1 Group:offset = MS: 6 */
          ,6,'b','o','u','n','d','s'
/* count=  1 Group:offset = MS:193 */
          ,6,'b','r','o','w','s','e'
/* count=  1 Group:offset = MS:208 */
          ,8,'c','a','l','l','-','r','e','t'
/* count=  1 Group:offset = MS:48 */
          ,16,'c','a','r','r','i','a','g','e',' '
              ,'c','o','n','t','r','o','l'
/* count=  1 Group:offset = MS:201 */
          ,13,'c','o','m','p','i','l','e',' ','o'
              ,'n','l','y',','
/* count=  1 Group:offset = MS:172 */
          ,12,'c','o','m','p','i','l','e','-','t'
              ,'i','m','e'
/* count=  1 Group:offset = MS:227 */
          ,9,'c','o','m','p','i','l','i','n','g'
/* count=  1 Group:offset = MS:188 */
          ,9,'c','o','n','s','t','a','n','t','s'
/* count=  1 Group:offset = MS:68 */
          ,12,'d','e','f','i','n','e',' ','m','a'
              ,'c','r','o'
/* count=  1 Group:offset = MS:17 */
          ,12,'d','e','p','e','n','d','e','n','c'
              ,'i','e','s'
/* count=  1 Group:offset = MS:198 */
          ,11,'d','e','s','c','r','i','p','t','o'
              ,'r','s'
/* count=  1 Group:offset = MS:65 */
          ,7,'d','e','v','i','c','e','s'
/* count=  1 Group:offset = MS:201 */
          ,10,'d','i','r','e','c','t','i','v','e'
              ,'s'
/* count=  1 Group:offset = MS:183 */
          ,7,'d','i','s','a','b','l','e'
/* count=  1 Group:offset = MS:49 */
          ,4,'d','i','s','k'
/* count=  1 Group:offset = MS: 5 */
          ,18,'d','i','s','p','l','a','y',' ','d'
              ,'i','a','g','n','o','s','t','i','c'
/* count=  1 Group:offset = MS:12 */
          ,7,'e','m','u','l','a','t','e'
/* count=  1 Group:offset = MS:39 */
          ,6,'e','n','a','b','l','e'
/* count=  1 Group:offset = MS:220 */
          ,10,'e','x','e','c','u','t','a','b','l'
              ,'e'
/* count=  1 Group:offset = MS:176 */
          ,24,'e','x','p','l','i','c','i','t',' '
              ,'t','y','p','i','n','g',' ','r','e','q'
              ,'u','i','r','e','d'
/* count=  1 Group:offset = MS:13 */
          ,9,'e','x','t','e','n','s','i','o','n'
/* count=  1 Group:offset = MS: 7 */
          ,4,'f','i','n','d'
/* count=  1 Group:offset = MS:191 */
          ,20,'f','l','o','a','t','i','n','g','-'
              ,'p','o','i','n','t',' ','c','a','l','l'
              ,'s'
/* count=  1 Group:offset = MS:38 */
          ,5,'f','r','a','m','e'
/* count=  1 Group:offset = MS:50 */
          ,4,'f','u','l','l'
/* count=  1 Group:offset = MS:46 */
          ,4,'h','u','g','e'
/* count=  1 Group:offset = MS:71 */
          ,19,'i','g','n','o','r','e',' ','W','F'
              ,'L',' ','v','a','r','i','a','b','l','e'
/* count=  1 Group:offset = MS:173 */
          ,7,'i','n','c','l','u','d','e'
/* count=  1 Group:offset = MS:178 */
          ,5,'i','n','f','o','.'
/* count=  1 Group:offset = MS:215 */
          ,11,'i','n','i','t','i','a','l','i','z'
              ,'e','d'
/* count=  1 Group:offset = MS:229 */
          ,22,'i','n','s','t','r','u','c','t','i'
              ,'o','n',' ','s','c','h','e','d','u','l'
              ,'i','n','g'
/* count=  1 Group:offset = MS:56 */
          ,9,'i','n','v','a','r','i','a','n','t'
/* count=  1 Group:offset = MS:229 */
          ,4,'k','e','e','p'
/* count=  1 Group:offset = MS:177 */
          ,5,'l','a','r','g','e'
/* count=  1 Group:offset = MS:70 */
          ,6,'l','e','n','g','t','h'
/* count=  1 Group:offset = MS:14 */
          ,9,'l','i','b','r','a','r','i','e','s'
/* count=  1 Group:offset = MS:37 */
          ,7,'l','i','n','k','i','n','g'
/* count=  1 Group:offset = MS:172 */
          ,4,'l','i','s','t'
/* count=  1 Group:offset = MS:11 */
          ,6,'m','a','n','g','l','e'
/* count=  1 Group:offset = MS:222 */
          ,3,'m','a','p'
/* count=  1 Group:offset = MS:175 */
          ,4,'m','a','t','h'
/* count=  1 Group:offset = MS:53 */
          ,6,'m','e','d','i','u','m'
/* count=  1 Group:offset = MS:69 */
          ,35,'m','o','v','e',' ','r','e','g','i'
              ,'s','t','e','r',' ','s','a','v','e','s'
              ,' ','i','n','t','o',' ','f','l','o','w'
              ,' ','g','r','a','p','h'
/* count=  1 Group:offset = MS:228 */
          ,5,'n','a','m','e','s'
/* count=  1 Group:offset = MS:222 */
          ,9,'n','u','m','e','r','i','c','a','l'
/* count=  1 Group:offset = MS:54 */
          ,6,'o','b','j','e','c','t'
/* count=  1 Group:offset = MS:67 */
          ,6,'o','n',' ','t','h','e'
/* count=  1 Group:offset = MS:64 */
          ,4,'o','n','l','y'
/* count=  1 Group:offset = MS:36 */
          ,15,'o','p','e','r','a','t','e',' ','q'
              ,'u','i','e','t','l','y'
/* count=  1 Group:offset = MS:202 */
          ,20,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s','/','a','s','s','u','m'
              ,'e'
/* count=  1 Group:offset = MS:229 */
          ,23,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s','/','u','n','r','o','l'
              ,'l','i','n','g'
/* count=  1 Group:offset = MS:210 */
          ,19,'o','p','t','i','m','i','z','e',' '
              ,'a','t',' ','e','x','p','e','n','s','e'
/* count=  1 Group:offset = MS:227 */
          ,8,'o','v','e','r','f','l','o','w'
/* count=  1 Group:offset = MS:218 */
          ,4,'p','a','s','s'
/* count=  1 Group:offset = MS:65 */
          ,4,'p','a','t','h'
/* count=  1 Group:offset = MS:219 */
          ,10,'p','r','e','d','i','c','t','i','o'
              ,'n'
/* count=  1 Group:offset = MS:221 */
          ,7,'p','r','i','n','t','e','r'
/* count=  1 Group:offset = MS: 3 */
          ,27,'p','r','o','m','o','t','e',' ','i'
              ,'n','t','r','i','n','s','i','c',' ','a'
              ,'r','g','u','m','e','n','t','s'
/* count=  1 Group:offset = MS:223 */
          ,4,'r','e','g','s'
/* count=  1 Group:offset = MS:91 */
          ,8,'r','e','s','o','u','r','c','e'
/* count=  1 Group:offset = MS:200 */
          ,21,'r','e','v','e','r','s','e',' ','c'
              ,'o','m','p','a','t','i','b','i','l','i'
              ,'t','y'
/* count=  1 Group:offset = MS:44 */
          ,18,'r','u','n',' ','p','r','o','t','e'
              ,'c','t','e','d',' ','m','o','d','e'
/* count=  1 Group:offset = MS:174 */
          ,18,'r','u','n','-','t','i','m','e',' '
              ,'t','r','a','c','e','b','a','c','k'
/* count=  1 Group:offset = MS:192 */
          ,12,'s','a','v','e','/','r','e','s','t'
              ,'o','r','e'
/* count=  1 Group:offset = MS:91 */
          ,8,'s','e','g','m','e','n','t','s'
/* count=  1 Group:offset = MS:211 */
          ,5,'s','p','a','c','e'
/* count=  1 Group:offset = MS:57 */
          ,16,'s','p','e','c','i','f','i','e','d'
              ,' ','s','y','s','t','e','m'
/* count=  1 Group:offset = MS:181 */
          ,19,'s','t','a','t','e','m','e','n','t'
              ,' ','f','u','n','c','t','i','o','n','s'
/* count=  1 Group:offset = MS:51 */
          ,6,'s','y','n','t','a','x'
/* count=  1 Group:offset = MS:36 */
          ,9,'t','e','m','p','o','r','a','r','y'
/* count=  1 Group:offset = MS:185 */
          ,8,'t','e','r','m','i','n','a','l'
/* count=  1 Group:offset = MS: 4 */
          ,9,'t','h','r','e','s','h','o','l','d'
/* count=  1 Group:offset = MS:63 */
          ,4,'t','i','m','e'
/* count=  1 Group:offset = MS:58 */
          ,12,'u','n','r','e','f','e','r','e','n'
              ,'c','e','d'
/* count=  1 Group:offset = MS: 8 */
          ,4,'w','i','l','d'
/* count=  1 Group:offset = MS:19 */
          ,20,'w','i','n','d','o','w','e','d',' '
              ,'a','p','p','l','i','c','a','t','i','o'
              ,'n'
/* count=  1 Group:offset = MS:77 */
                   };
/* Total number of phrases = 189 */
