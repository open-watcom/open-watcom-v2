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
,4,2,16,20,0
/* listing file to printer  */
,4,20,0,3,172
/* listing file to terminal  */
,4,20,0,3,187
/* listing file to disk  */
,4,20,0,3,127
/* generate an error file  */
,3,2,106,0
/* issue extension messages  */
,3,29,134,22
/* issue unreferenced warning  */
,3,29,189,64
/* issue warning messages  */
,3,29,64,22
/* INTEGER/LOGICAL size 2/1  */
,3,93,63,79
/* list INCLUDE files  */
,3,153,38,27
/* display diagnostic messages  */
,2,128,22
/* explicit typing required  */
,1,133
/* extend line length  */
,3,48,53,149
,0
/* /trace /bounds  */
,1,78
/* define macro  */
,1,121
/* relax FORMAT type checking  */
,3,62,90,18
/* relax wild branch checking  */
,4,62,190,42,18
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
,3,185,43,163
/* generate default libraries  */
,3,2,46,150
/* floating-point calls  */
,1,137
/* emulate 80x87 instructions  */
,3,130,25,19
/* in-line 80x87 instructions  */
,3,52,25,19
/* 80287 instructions  */
,2,80,19
/* 80387 instructions  */
,2,81,19
/* /fp3 optimized for Pentium  */
,4,35,14,5,10
/* 80x87 reverse compatibility  */
,2,25,176
/* line # debugging information  */
,4,53,65,11,12
/* full debugging information  */
,3,139,11,12
/* base pointer optimizations  */
,2,109,1
/* no call-ret optimizations  */
,3,30,112,1
/* disable optimizations  */
,2,126,1
/* no stack frame optimizations  */
,4,30,9,138,1
/* statement functions in-line  */
,2,184,52
/* loop optimizations  */
,2,21,1
/* math optimizations  */
,2,156,1
/* numerical optimizations  */
,2,161,1
/* precision optimizations  */
,2,61,1
/* instruction scheduling  */
,1,145
/* space optimizations  */
,2,182,1
/* time optimizations  */
,2,188,1
/* /o[b,bp,i,k,l,m,r,t,do]  */
,1,77
,0
,0
/* stack checking  */
,2,9,18
/* set data threshold  */
,2,15,120
/* local variables on the stack  */
,4,56,34,162,9
/* pass character descriptors  */
,3,169,17,123
/* SAVE local variables  */
,3,99,56,34
/* name object file  */
,3,57,58,0
/* constants in code segment  */
,4,119,51,44,33
,0
,0
,0
,0
,0
,0
/* stack calling convention  */
,2,9,113
/* Easy OMF-386 object files  */
,3,88,58,27
/* default windowed application  */
,3,46,191,40
/* multithread application  */
,2,159,40
,0
/* dynamic link library  */
,3,129,54,151
,0
/* automatic stack growing  */
,3,108,9,140
/* flat memory model  */
,3,136,8,13
/* small memory model  */
,3,181,8,13
/* compact memory model  */
,3,115,8,13
/* medium memory model  */
,3,157,8,13
/* large memory model  */
,3,148,8,13
/* assume 80386 processor  */
,1,107
/* /3 optimized for 80486  */
,4,24,14,5,82
/* /3 optimized for Pentium  */
,4,24,14,5,10
/* save/restore segment regs  */
,3,179,33,174
/* FS not fixed  */
,3,91,31,49
/* GS not fixed  */
,3,92,31,49
,0
/* generate code for Windows  */
,4,2,44,5,102
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
,0
/* Usage: wfl386 {- or /<option> | <file>}  */
,1,100
/* compile only, no linking  */
,3,116,30,152
/* ignore WFL variable  */
,1,141
/* run protected mode compiler  */
,2,177,45
/* generate map file  */
,3,2,155,0
/* name executable file  */
,3,57,132,0
/* keep directive file  */
,3,147,47,0
/* include directive file  */
,3,142,47,0
,0
,0
/* link for specified system  */
,3,54,5,183
/* set stack size  */
,3,15,9,63
/* additional linker directives  */
,3,103,55,125
/* Out of memory  */
,3,98,59,8
/* Error: Unable to open temporary file  */
,6,4,7,3,60,186,0
/* Unable to open '%s1'  */
,4,7,3,60,23
/* Error: Unable to invoke compiler  */
,5,4,7,3,28,45
/* Error: Compiler returned a bad status compiling '%s1'  */
,7,4,85,32,16,26,118,23
/* Error: Unable to invoke linker  */
,5,4,7,3,28,55
/* Error: Linker returned a bad status  */
,5,4,97,32,16,26
/* Error: Unable to find '%s1'  */
,5,4,7,3,135,23
/* generate run-time traceback  */
,2,2,178
/* generate bounds checking  */
,3,2,110,18
/* *Character Set Options*  */
,2,68,6
/* Chinese character set  */
,3,83,17,15
/* Japanese character set  */
,3,94,17,15
/* Korean character set  */
,3,95,17,15
/* generate file dependencies  */
,3,2,0,122
/* *Application Type*  */
,1,66
/* messages in resource file  */
,4,22,51,175,0
/* devices are carriage control  */
,3,124,41,114
/* operate quietly  */
,1,164
,0
,0
,0
,0
,0
/* generate browse information  */
,3,2,111,12
/* extend float-pt. precision  */
,3,48,50,61
/* loop optimizations/unrolling  */
,2,21,166
/* align COMMON segments  */
,3,104,36,180
/* LF with FF  */
,1,96
/* WATCOM debugging information  */
,3,101,11,12
/* DWARF debugging information  */
,3,87,11,12
/* Codeview debugging info.  */
,3,84,11,143
/* Error: Unable to invoke CVPACK  */
,5,4,7,3,28,37
/* Error: CVPACK returned a bad status  */
,5,4,37,32,16,26
/* DO-variables do not overflow  */
,3,86,31,168
/* path for INCLUDE files  */
,4,170,5,38,27
/* enable Pentium FDIV check  */
,4,131,10,89,43
/* branch prediction  */
,2,42,171
/* mangle COMMON segment names  */
,4,154,36,33,160
/* promote intrinsic arguments  */
,1,173
/* allow comma separator  */
,1,105
/* /fp3 optimized for Pentium Pro  */
,5,35,14,5,10,39
/* /3 optimized for Pentium Pro  */
,5,24,14,5,10,39
/* optimize at expense of compile-time  */
,3,167,59,117
/* move register saves into flow graph  */
,1,158
/* loop optimizations/assume loop invariant float-pt. variables are initialized  */
,8,21,165,21,146,50,34,41,144
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
          ,13,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s'
/* count= 10 Group:offset = MS:47, MS:48, MS:49, MS:50, MS:52,... */
          ,8,'g','e','n','e','r','a','t','e'
/* count=  9 Group:offset = MS: 2, MS: 6, MS:37, MS:95, MS:175,... */
          ,2,'t','o'
/* count=  9 Group:offset = MS: 3, MS: 4, MS: 5, MS:185, MS:186,... */
          ,6,'E','r','r','o','r',':'
/* count=  8 Group:offset = MS:185, MS:187, MS:188, MS:189, MS:190,... */
          ,3,'f','o','r'
/* count=  8 Group:offset = MS:43, MS:89, MS:90, MS:95, MS:181,... */
          ,8,'O','p','t','i','o','n','s','*'
/* count=  7 Group:offset = MS:161, MS:162, MS:163, MS:164, MS:165,... */
          ,6,'U','n','a','b','l','e'
/* count=  6 Group:offset = MS:185, MS:186, MS:187, MS:189, MS:191,... */
          ,6,'m','e','m','o','r','y'
/* count=  6 Group:offset = MS:83, MS:84, MS:85, MS:86, MS:87,... */
          ,5,'s','t','a','c','k'
/* count=  6 Group:offset = MS:50, MS:62, MS:64, MS:75, MS:82,... */
          ,7,'P','e','n','t','i','u','m'
/* count=  5 Group:offset = MS:43, MS:90, MS:220, MS:225, MS:226 */
          ,9,'d','e','b','u','g','g','i','n','g'
/* count=  5 Group:offset = MS:45, MS:46, MS:213, MS:214, MS:215 */
          ,11,'i','n','f','o','r','m','a','t','i'
              ,'o','n'
/* count=  5 Group:offset = MS:45, MS:46, MS:208, MS:213, MS:214 */
          ,5,'m','o','d','e','l'
/* count=  5 Group:offset = MS:83, MS:84, MS:85, MS:86, MS:87 */
          ,9,'o','p','t','i','m','i','z','e','d'
/* count=  5 Group:offset = MS:43, MS:89, MS:90, MS:225, MS:226 */
          ,3,'s','e','t'
/* count=  5 Group:offset = MS:63, MS:182, MS:195, MS:196, MS:197 */
          ,1,'a'
/* count=  4 Group:offset = MS: 2, MS:188, MS:190, MS:217 */
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
          ,8,'m','e','s','s','a','g','e','s'
/* count=  4 Group:offset = MS: 7, MS: 9, MS:12, MS:200 */
          ,5,'\'','%','s','1','\''
/* count=  3 Group:offset = MS:186, MS:188, MS:191 */
          ,2,'/','3'
/* count=  3 Group:offset = MS:89, MS:90, MS:226 */
          ,5,'8','0','x','8','7'
/* count=  3 Group:offset = MS:39, MS:40, MS:44 */
          ,10,'b','a','d',' ','s','t','a','t','u'
              ,'s'
/* count=  3 Group:offset = MS:188, MS:190, MS:217 */
          ,5,'f','i','l','e','s'
/* count=  3 Group:offset = MS:11, MS:76, MS:219 */
          ,6,'i','n','v','o','k','e'
/* count=  3 Group:offset = MS:187, MS:189, MS:216 */
          ,5,'i','s','s','u','e'
/* count=  3 Group:offset = MS: 7, MS: 8, MS: 9 */
          ,2,'n','o'
/* count=  3 Group:offset = MS:48, MS:50, MS:172 */
          ,3,'n','o','t'
/* count=  3 Group:offset = MS:92, MS:93, MS:218 */
          ,8,'r','e','t','u','r','n','e','d'
/* count=  3 Group:offset = MS:188, MS:190, MS:217 */
          ,7,'s','e','g','m','e','n','t'
/* count=  3 Group:offset = MS:68, MS:91, MS:222 */
          ,9,'v','a','r','i','a','b','l','e','s'
/* count=  3 Group:offset = MS:64, MS:66, MS:229 */
          ,4,'/','f','p','3'
/* count=  2 Group:offset = MS:43, MS:225 */
          ,6,'C','O','M','M','O','N'
/* count=  2 Group:offset = MS:211, MS:222 */
          ,6,'C','V','P','A','C','K'
/* count=  2 Group:offset = MS:216, MS:217 */
          ,7,'I','N','C','L','U','D','E'
/* count=  2 Group:offset = MS:11, MS:219 */
          ,3,'P','r','o'
/* count=  2 Group:offset = MS:225, MS:226 */
          ,11,'a','p','p','l','i','c','a','t','i'
              ,'o','n'
/* count=  2 Group:offset = MS:77, MS:78 */
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
          ,7,'d','e','f','a','u','l','t'
/* count=  2 Group:offset = MS:37, MS:77 */
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
          ,4,'l','i','n','k'
/* count=  2 Group:offset = MS:80, MS:181 */
          ,6,'l','i','n','k','e','r'
/* count=  2 Group:offset = MS:183, MS:189 */
          ,5,'l','o','c','a','l'
/* count=  2 Group:offset = MS:64, MS:66 */
          ,4,'n','a','m','e'
/* count=  2 Group:offset = MS:67, MS:176 */
          ,6,'o','b','j','e','c','t'
/* count=  2 Group:offset = MS:67, MS:76 */
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
          ,23,'/','o','[','b',',','b','p',',','i'
              ,',','k',',','l',',','m',',','r',',','t'
              ,',','d','o',']'
/* count=  1 Group:offset = MS:59 */
          ,14,'/','t','r','a','c','e',' ','/','b'
              ,'o','u','n','d','s'
/* count=  1 Group:offset = MS:16 */
          ,3,'2','/','1'
/* count=  1 Group:offset = MS:10 */
          ,5,'8','0','2','8','7'
/* count=  1 Group:offset = MS:41 */
          ,5,'8','0','3','8','7'
/* count=  1 Group:offset = MS:42 */
          ,5,'8','0','4','8','6'
/* count=  1 Group:offset = MS:89 */
          ,7,'C','h','i','n','e','s','e'
/* count=  1 Group:offset = MS:195 */
          ,8,'C','o','d','e','v','i','e','w'
/* count=  1 Group:offset = MS:215 */
          ,8,'C','o','m','p','i','l','e','r'
/* count=  1 Group:offset = MS:188 */
          ,15,'D','O','-','v','a','r','i','a','b'
              ,'l','e','s',' ','d','o'
/* count=  1 Group:offset = MS:218 */
          ,5,'D','W','A','R','F'
/* count=  1 Group:offset = MS:214 */
          ,12,'E','a','s','y',' ','O','M','F','-'
              ,'3','8','6'
/* count=  1 Group:offset = MS:76 */
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
          ,3,'O','u','t'
/* count=  1 Group:offset = MS:184 */
          ,4,'S','A','V','E'
/* count=  1 Group:offset = MS:66 */
          ,39,'U','s','a','g','e',':',' ','w','f'
              ,'l','3','8','6',' ','{','-',' ','o','r'
              ,' ','/','<','o','p','t','i','o','n','>'
              ,' ','|',' ','<','f','i','l','e','>','}'
/* count=  1 Group:offset = MS:171 */
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
          ,22,'a','s','s','u','m','e',' ','8','0'
              ,'3','8','6',' ','p','r','o','c','e','s'
              ,'s','o','r'
/* count=  1 Group:offset = MS:88 */
          ,9,'a','u','t','o','m','a','t','i','c'
/* count=  1 Group:offset = MS:82 */
          ,12,'b','a','s','e',' ','p','o','i','n'
              ,'t','e','r'
/* count=  1 Group:offset = MS:47 */
          ,6,'b','o','u','n','d','s'
/* count=  1 Group:offset = MS:193 */
          ,6,'b','r','o','w','s','e'
/* count=  1 Group:offset = MS:208 */
          ,8,'c','a','l','l','-','r','e','t'
/* count=  1 Group:offset = MS:48 */
          ,18,'c','a','l','l','i','n','g',' ','c'
              ,'o','n','v','e','n','t','i','o','n'
/* count=  1 Group:offset = MS:75 */
          ,16,'c','a','r','r','i','a','g','e',' '
              ,'c','o','n','t','r','o','l'
/* count=  1 Group:offset = MS:201 */
          ,7,'c','o','m','p','a','c','t'
/* count=  1 Group:offset = MS:85 */
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
          ,14,'d','a','t','a',' ','t','h','r','e'
              ,'s','h','o','l','d'
/* count=  1 Group:offset = MS:63 */
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
          ,7,'d','y','n','a','m','i','c'
/* count=  1 Group:offset = MS:80 */
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
          ,4,'f','l','a','t'
/* count=  1 Group:offset = MS:83 */
          ,20,'f','l','o','a','t','i','n','g','-'
              ,'p','o','i','n','t',' ','c','a','l','l'
              ,'s'
/* count=  1 Group:offset = MS:38 */
          ,5,'f','r','a','m','e'
/* count=  1 Group:offset = MS:50 */
          ,4,'f','u','l','l'
/* count=  1 Group:offset = MS:46 */
          ,7,'g','r','o','w','i','n','g'
/* count=  1 Group:offset = MS:82 */
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
/* count=  1 Group:offset = MS:87 */
          ,6,'l','e','n','g','t','h'
/* count=  1 Group:offset = MS:14 */
          ,9,'l','i','b','r','a','r','i','e','s'
/* count=  1 Group:offset = MS:37 */
          ,7,'l','i','b','r','a','r','y'
/* count=  1 Group:offset = MS:80 */
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
/* count=  1 Group:offset = MS:86 */
          ,35,'m','o','v','e',' ','r','e','g','i'
              ,'s','t','e','r',' ','s','a','v','e','s'
              ,' ','i','n','t','o',' ','f','l','o','w'
              ,' ','g','r','a','p','h'
/* count=  1 Group:offset = MS:228 */
          ,11,'m','u','l','t','i','t','h','r','e'
              ,'a','d'
/* count=  1 Group:offset = MS:78 */
          ,5,'n','a','m','e','s'
/* count=  1 Group:offset = MS:222 */
          ,9,'n','u','m','e','r','i','c','a','l'
/* count=  1 Group:offset = MS:54 */
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
          ,5,'s','m','a','l','l'
/* count=  1 Group:offset = MS:84 */
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
          ,4,'t','i','m','e'
/* count=  1 Group:offset = MS:58 */
          ,12,'u','n','r','e','f','e','r','e','n'
              ,'c','e','d'
/* count=  1 Group:offset = MS: 8 */
          ,4,'w','i','l','d'
/* count=  1 Group:offset = MS:19 */
          ,8,'w','i','n','d','o','w','e','d'
/* count=  1 Group:offset = MS:77 */
                   };
/* Total number of phrases = 192 */
