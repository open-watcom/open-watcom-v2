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
,4,3,10,12,0
/* listing file to printer  */
,4,12,0,1,131
/* listing file to terminal  */
,4,12,0,1,142
/* listing file to disk  */
,4,12,0,1,94
/* generate an error file  */
,3,3,79,0
/* issue extension messages  */
,3,19,99,14
/* issue unreferenced warning  */
,3,19,144,46
/* issue warning messages  */
,3,19,46,14
/* INTEGER/LOGICAL size 2/1  */
,3,68,44,61
/* list INCLUDE files  */
,2,114,24
/* display diagnostic messages  */
,2,95,14
/* explicit typing required  */
,1,98
/* extend line length  */
,3,31,34,110
,0
/* /trace /bounds  */
,1,60
/* define macro  */
,1,88
/* relax FORMAT type checking  */
,3,43,67,17
/* relax wild branch checking  */
,4,43,145,27,17
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
,1,140
/* generate default libraries  */
,3,3,29,111
,0
,0
,0
,0
,0
,0
,0
/* line # debugging information  */
,4,34,47,7,8
/* full debugging information  */
,3,101,7,8
,0
/* no call-ret optimizations  */
,3,39,82,4
/* disable optimizations  */
,2,93,4
,0
/* statement functions in-line  */
,1,139
/* loop optimizations  */
,2,13,4
/* math optimizations  */
,2,117,4
/* numerical optimizations  */
,2,121,4
/* precision optimizations  */
,2,42,4
/* instruction scheduling  */
,1,107
/* space optimizations  */
,2,137,4
/* time optimizations  */
,2,143,4
,0
,0
/* /o[bp,i,l,k,m,r,t,do]  */
,1,59
,0
/* set data threshold  */
,2,9,87
/* local variables on the stack  */
,4,37,21,123,45
/* pass character descriptors  */
,3,128,11,90
/* SAVE local variables  */
,3,74,37,21
/* name object file  */
,3,38,122,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* default windowed application  */
,3,29,146,25
,0
/* multithread application  */
,2,120,25
,0
/* dynamic link library  */
,3,96,35,112
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
,2,54,5
/* *Diagnostic Options*  */
,2,53,5
/* *Debugging Options*  */
,2,52,5
/* *Miscellaneous Options*  */
,2,57,5
/* *Floating-Point Options*  */
,2,55,5
/* *Optimizations*  */
,1,58
/* *Memory Models*  */
,1,56
/* *CPU Targets*  */
,1,49
/* *Compile and Link Options*  */
,2,51,5
,0
,0
/* compile only, no linking  */
,3,84,39,113
/* ignore WFL variable  */
,1,102
/* run protected mode compiler  */
,2,133,28
/* generate map file  */
,3,3,116,0
/* name executable file  */
,3,38,97,0
/* keep directive file  */
,3,109,30,0
/* include directive file  */
,3,104,30,0
,0
,0
/* link for specified system  */
,3,35,33,138
/* set stack size  */
,3,9,45,44
/* additional linker directives  */
,3,76,36,92
/* Out of memory  */
,3,73,40,118
/* Error: Unable to open temporary file  */
,6,2,6,1,41,141,0
/* Unable to open '%s1'  */
,4,6,1,41,15
/* Error: Unable to invoke compiler  */
,5,2,6,1,18,28
/* Error: Compiler returned a bad status compiling '%s1'  */
,7,2,64,20,10,16,86,15
/* Error: Unable to invoke linker  */
,5,2,6,1,18,36
/* Error: Linker returned a bad status  */
,5,2,72,20,10,16
/* Error: Unable to find '%s1'  */
,5,2,6,1,100,15
/* generate run-time traceback  */
,2,3,134
/* generate bounds checking  */
,3,3,80,17
/* *Character Set Options*  */
,2,50,5
/* Chinese character set  */
,3,62,11,9
/* Japanese character set  */
,3,69,11,9
/* Korean character set  */
,3,70,11,9
/* generate file dependencies  */
,3,3,0,89
/* *Application Type*  */
,1,48
/* messages in resource file  */
,3,14,103,0
/* devices are carriage control  */
,3,91,26,83
/* operate quietly  */
,1,124
,0
,0
,0
,0
,0
/* generate browse information  */
,3,3,81,8
/* extend float-pt. precision  */
,3,31,32,42
/* loop optimizations/unrolling  */
,2,13,126
/* align COMMON segments  */
,3,77,22,136
/* LF with FF  */
,1,71
/* WATCOM debugging information  */
,3,75,7,8
/* DWARF debugging information  */
,3,66,7,8
/* Codeview debugging info.  */
,3,63,7,105
/* Error: Unable to invoke CVPACK  */
,5,2,6,1,18,23
/* Error: CVPACK returned a bad status  */
,5,2,23,20,10,16
/* DO-variables do not overflow  */
,1,65
/* path for INCLUDE files  */
,3,129,33,24
,0
/* branch prediction  */
,2,27,130
/* mangle COMMON segment names  */
,3,115,22,135
/* promote intrinsic arguments  */
,1,132
/* allow comma separator  */
,1,78
,0
,0
/* optimize at expense of compile-time  */
,3,127,40,85
/* move register saves into flow graph  */
,1,119
/* loop optimizations/assume loop invariant float-pt. variables are initialized  */
,8,13,125,13,108,32,21,26,106
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
          ,2,'t','o'
/* count=  9 Group:offset = MS: 3, MS: 4, MS: 5, MS:185, MS:186,... */
          ,6,'E','r','r','o','r',':'
/* count=  8 Group:offset = MS:185, MS:187, MS:188, MS:189, MS:190,... */
          ,8,'g','e','n','e','r','a','t','e'
/* count=  8 Group:offset = MS: 2, MS: 6, MS:37, MS:175, MS:192,... */
          ,13,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s'
/* count=  8 Group:offset = MS:48, MS:49, MS:52, MS:53, MS:54,... */
          ,8,'O','p','t','i','o','n','s','*'
/* count=  7 Group:offset = MS:161, MS:162, MS:163, MS:164, MS:165,... */
          ,6,'U','n','a','b','l','e'
/* count=  6 Group:offset = MS:185, MS:186, MS:187, MS:189, MS:191,... */
          ,9,'d','e','b','u','g','g','i','n','g'
/* count=  5 Group:offset = MS:45, MS:46, MS:213, MS:214, MS:215 */
          ,11,'i','n','f','o','r','m','a','t','i'
              ,'o','n'
/* count=  5 Group:offset = MS:45, MS:46, MS:208, MS:213, MS:214 */
          ,3,'s','e','t'
/* count=  5 Group:offset = MS:63, MS:182, MS:195, MS:196, MS:197 */
          ,1,'a'
/* count=  4 Group:offset = MS: 2, MS:188, MS:190, MS:217 */
          ,9,'c','h','a','r','a','c','t','e','r'
/* count=  4 Group:offset = MS:65, MS:195, MS:196, MS:197 */
          ,7,'l','i','s','t','i','n','g'
/* count=  4 Group:offset = MS: 2, MS: 3, MS: 4, MS: 5 */
          ,4,'l','o','o','p'
/* count=  4 Group:offset = MS:52, MS:210, MS:229 */
          ,8,'m','e','s','s','a','g','e','s'
/* count=  4 Group:offset = MS: 7, MS: 9, MS:12, MS:200 */
          ,5,'\'','%','s','1','\''
/* count=  3 Group:offset = MS:186, MS:188, MS:191 */
          ,10,'b','a','d',' ','s','t','a','t','u'
              ,'s'
/* count=  3 Group:offset = MS:188, MS:190, MS:217 */
          ,8,'c','h','e','c','k','i','n','g'
/* count=  3 Group:offset = MS:18, MS:19, MS:193 */
          ,6,'i','n','v','o','k','e'
/* count=  3 Group:offset = MS:187, MS:189, MS:216 */
          ,5,'i','s','s','u','e'
/* count=  3 Group:offset = MS: 7, MS: 8, MS: 9 */
          ,8,'r','e','t','u','r','n','e','d'
/* count=  3 Group:offset = MS:188, MS:190, MS:217 */
          ,9,'v','a','r','i','a','b','l','e','s'
/* count=  3 Group:offset = MS:64, MS:66, MS:229 */
          ,6,'C','O','M','M','O','N'
/* count=  2 Group:offset = MS:211, MS:222 */
          ,6,'C','V','P','A','C','K'
/* count=  2 Group:offset = MS:216, MS:217 */
          ,13,'I','N','C','L','U','D','E',' ','f'
              ,'i','l','e','s'
/* count=  2 Group:offset = MS:11, MS:219 */
          ,11,'a','p','p','l','i','c','a','t','i'
              ,'o','n'
/* count=  2 Group:offset = MS:77, MS:79 */
          ,3,'a','r','e'
/* count=  2 Group:offset = MS:201, MS:229 */
          ,6,'b','r','a','n','c','h'
/* count=  2 Group:offset = MS:19, MS:221 */
          ,8,'c','o','m','p','i','l','e','r'
/* count=  2 Group:offset = MS:174, MS:187 */
          ,7,'d','e','f','a','u','l','t'
/* count=  2 Group:offset = MS:37, MS:77 */
          ,9,'d','i','r','e','c','t','i','v','e'
/* count=  2 Group:offset = MS:177, MS:178 */
          ,6,'e','x','t','e','n','d'
/* count=  2 Group:offset = MS:14, MS:209 */
          ,9,'f','l','o','a','t','-','p','t','.'
/* count=  2 Group:offset = MS:209, MS:229 */
          ,3,'f','o','r'
/* count=  2 Group:offset = MS:181, MS:219 */
          ,4,'l','i','n','e'
/* count=  2 Group:offset = MS:14, MS:45 */
          ,4,'l','i','n','k'
/* count=  2 Group:offset = MS:81, MS:181 */
          ,6,'l','i','n','k','e','r'
/* count=  2 Group:offset = MS:183, MS:189 */
          ,5,'l','o','c','a','l'
/* count=  2 Group:offset = MS:64, MS:66 */
          ,4,'n','a','m','e'
/* count=  2 Group:offset = MS:67, MS:176 */
          ,2,'n','o'
/* count=  2 Group:offset = MS:48, MS:172 */
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
          ,5,'s','t','a','c','k'
/* count=  2 Group:offset = MS:64, MS:182 */
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
/* count=  1 Group:offset = MS:61 */
          ,14,'/','t','r','a','c','e',' ','/','b'
              ,'o','u','n','d','s'
/* count=  1 Group:offset = MS:16 */
          ,3,'2','/','1'
/* count=  1 Group:offset = MS:10 */
          ,7,'C','h','i','n','e','s','e'
/* count=  1 Group:offset = MS:195 */
          ,8,'C','o','d','e','v','i','e','w'
/* count=  1 Group:offset = MS:215 */
          ,8,'C','o','m','p','i','l','e','r'
/* count=  1 Group:offset = MS:188 */
          ,28,'D','O','-','v','a','r','i','a','b'
              ,'l','e','s',' ','d','o',' ','n','o','t'
              ,' ','o','v','e','r','f','l','o','w'
/* count=  1 Group:offset = MS:218 */
          ,5,'D','W','A','R','F'
/* count=  1 Group:offset = MS:214 */
          ,11,'F','O','R','M','A','T',' ','t','y'
              ,'p','e'
/* count=  1 Group:offset = MS:18 */
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
          ,6,'W','A','T','C','O','M'
/* count=  1 Group:offset = MS:213 */
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
/* count=  1 Group:offset = MS:81 */
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
          ,4,'f','u','l','l'
/* count=  1 Group:offset = MS:46 */
          ,19,'i','g','n','o','r','e',' ','W','F'
              ,'L',' ','v','a','r','i','a','b','l','e'
/* count=  1 Group:offset = MS:173 */
          ,11,'i','n',' ','r','e','s','o','u','r'
              ,'c','e'
/* count=  1 Group:offset = MS:200 */
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
          ,6,'l','e','n','g','t','h'
/* count=  1 Group:offset = MS:14 */
          ,9,'l','i','b','r','a','r','i','e','s'
/* count=  1 Group:offset = MS:37 */
          ,7,'l','i','b','r','a','r','y'
/* count=  1 Group:offset = MS:81 */
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
          ,6,'m','e','m','o','r','y'
/* count=  1 Group:offset = MS:184 */
          ,35,'m','o','v','e',' ','r','e','g','i'
              ,'s','t','e','r',' ','s','a','v','e','s'
              ,' ','i','n','t','o',' ','f','l','o','w'
              ,' ','g','r','a','p','h'
/* count=  1 Group:offset = MS:228 */
          ,11,'m','u','l','t','i','t','h','r','e'
              ,'a','d'
/* count=  1 Group:offset = MS:79 */
          ,9,'n','u','m','e','r','i','c','a','l'
/* count=  1 Group:offset = MS:54 */
          ,6,'o','b','j','e','c','t'
/* count=  1 Group:offset = MS:67 */
          ,6,'o','n',' ','t','h','e'
/* count=  1 Group:offset = MS:64 */
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
          ,18,'r','u','n',' ','p','r','o','t','e'
              ,'c','t','e','d',' ','m','o','d','e'
/* count=  1 Group:offset = MS:174 */
          ,18,'r','u','n','-','t','i','m','e',' '
              ,'t','r','a','c','e','b','a','c','k'
/* count=  1 Group:offset = MS:192 */
          ,13,'s','e','g','m','e','n','t',' ','n'
              ,'a','m','e','s'
/* count=  1 Group:offset = MS:222 */
          ,8,'s','e','g','m','e','n','t','s'
/* count=  1 Group:offset = MS:211 */
          ,5,'s','p','a','c','e'
/* count=  1 Group:offset = MS:57 */
          ,16,'s','p','e','c','i','f','i','e','d'
              ,' ','s','y','s','t','e','m'
/* count=  1 Group:offset = MS:181 */
          ,27,'s','t','a','t','e','m','e','n','t'
              ,' ','f','u','n','c','t','i','o','n','s'
              ,' ','i','n','-','l','i','n','e'
/* count=  1 Group:offset = MS:51 */
          ,17,'s','y','n','t','a','x',' ','c','h'
              ,'e','c','k',' ','o','n','l','y'
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
/* Total number of phrases = 147 */
