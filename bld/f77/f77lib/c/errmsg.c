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

static const char __FAR Msg1536[] = {
/* program abnormally terminated  */
 2,81,120
};
static const char __FAR Msg3840[] = {
/* zero**J where J <= 0 is not allowed  */
 6,181,57,111,1,0,11
/* X**Y where X < 0.0, Y is not of type INTEGER, is not allowed  */
,13,116,57,61,96,62,1,0,6,54,110,1,0,11
/* (0,0)**Y where Y is not real is not allowed  */
,9,92,57,62,1,0,162,1,0,11
};
static const char __FAR Msg4352[] = {
/* missing delimiter in format string, comma assumed  */
 6,9,138,5,4,167,66
/* missing or invalid constant  */
,4,9,10,2,69
/* Ew.dDe format code is not FORTRAN 77 standard  */
,6,105,4,12,1,0,17
/* missing decimal point  */
,2,9,137
/* missing or invalid edit descriptor in format string  */
,8,9,10,2,13,22,5,4,83
/* unrecognizable edit descriptor in format string  */
,6,179,13,22,5,4,83
/* invalid repeat specification  */
,3,2,82,24
/* $ or \ format code is not FORTRAN 77 standard  */
,8,89,10,118,4,12,1,0,17
/* invalid field modifier  */
,3,2,30,158
,0
/* repeat specification not allowed for this format code  */
,8,82,24,0,11,32,173,4,12
,0
/* no closing quote on apostrophe edit descriptor  */
,6,77,132,78,125,13,22
/* field count greater than 256 is invalid  */
,6,30,135,48,94,1,2
/* invalid field width specification  */
,4,2,30,180,24
/* Z format code is not FORTRAN 77 standard  */
,6,117,4,12,1,0,17
/* FORMAT statement exceeds allotted storage size  */
,4,59,16,142,53
/* format specification not allowed on input  */
,6,4,24,0,11,78,33
/* FORMAT missing repeatable edit descriptor  */
,5,59,9,163,13,22
/* missing constant before X edit descriptor, 1 assumed  */
,7,9,69,130,61,13,139,66
/* Ew.dQe format code is not FORTRAN 77 standard  */
,6,106,4,12,1,0,17
/* Qw.d format code is not FORTRAN 77 standard  */
,6,113,4,12,1,0,17
};
static const char __FAR Msg5888[] = {
/* BACKSPACE statement requires sequential access mode  */
 6,97,16,15,36,18,76
/* input/output is already active  */
,4,14,1,63,122
/* ENDFILE statement requires sequential access mode  */
,6,104,16,15,36,18,76
/* formatted connection requires formatted input/output statements  */
,6,47,68,15,47,14,37
/* unformatted connection requires unformatted input/output statements  */
,6,55,68,15,55,14,37
/* REWIND statement requires sequential access  */
,5,114,16,15,36,18
/* bad character in input field  */
,5,129,43,5,33,30
/* BLANK specifier requires FORM specifier to be 'FORMATTED'  */
,8,98,3,15,108,3,7,20,90
/* system file error - %s1  */
,5,172,8,29,38,26
/* format specification does not match data type  */
,7,4,24,27,0,75,21,54
/* input item does not match the data type of list variable  */
,11,33,150,27,0,75,86,21,54,6,154,56
/* internal file is full  */
,4,148,8,1,145
/* RECL specifier is invalid  */
,4,60,3,1,2
/* invalid STATUS specifier in CLOSE statement  */
,6,2,41,3,5,102,16
/* unit specified is not connected  */
,4,87,1,0,133
/* attempt to perform data transfer past end of file  */
,7,42,7,160,21,175,6,8
/* invalid RECL specifier/ACCESS specifier combination  */
,5,2,60,165,3,44
/* REC specifier required in direct access input/output statements  */
,8,40,3,164,5,140,18,14,37
/* REC specifier not allowed in sequential access input/output statements  */
,9,40,3,0,11,5,36,18,14,37
/* %s1 specifier may not change in a subsequent OPEN statement  */
,8,26,3,155,0,131,5,119,16
/* invalid STATUS specifier for given file  */
,6,2,41,3,32,146,8
/* invalid STATUS specifier/FILE specifier combination  */
,5,2,41,166,3,44
/* record size exceeded during unformatted input/output  */
,6,35,53,45,28,55,14
/* unit specified does not exist  */
,4,87,27,0,72
/* REC specifier is invalid  */
,4,40,3,1,2
/* UNIT specifier is invalid  */
,4,115,3,1,2
/* formatted record or format edit descriptor is too large for record size  */
,11,47,35,10,4,13,22,1,174,32,35,53
/* illegal '%s1=' specifier  */
,2,147,3
/* invalid CARRIAGECONTROL specifier/FORM specifier combination  */
,4,2,100,3,44
/* i/o operation not consistent with file attributes  */
,7,49,80,0,134,88,8,128
/* symbol %s1 not found in NAMELIST  */
,6,170,26,0,144,5,112
/* syntax error during NAMELIST-directed input  */
,5,171,29,28,39,33
/* subscripting error during NAMELIST-directed i/o  */
,5,169,29,28,39,49
/* substring error during NAMELIST-directed i/o  */
,5,85,29,28,39,49
/* BLOCKSIZE specifier is invalid  */
,4,99,3,1,2
/* invalid operation for files with no record structure  */
,8,2,80,32,143,88,77,35,168
/* integer overflow converting character data to integer  */
,7,34,51,70,43,21,7,34
/* range exceeded converting character data to floating-point  */
,7,52,45,70,43,21,7,31
};
static const char __FAR Msg6144[] = {
/* floating-point divide by zero  */
 3,31,71,25
/* floating-point overflow  */
,2,31,51
/* floating-point underflow  */
,2,31,177
/* integer divide by zero  */
,3,34,71,25
/* program interrupted from keyboard  */
,4,81,149,73,151
/* integer overflow  */
,2,34,51
};
static const char __FAR Msg6400[] = {
/* argument must be greater than zero  */
 5,19,23,20,48,25
/* absolute value of argument to arcsine, arccosine must not exceed one  */
,9,121,6,19,7,126,23,0,141,79
/* argument must not be negative  */
,5,19,23,0,20,159
/* argument(s) must not be zero  */
,5,127,23,0,20,25
/* argument of CHAR must be in the range zero to 255  */
,11,19,6,101,23,20,5,86,52,25,7,93
,0
,0
,0
/* length of ICHAR argument greater than one  */
,6,152,6,109,19,48,79
};
static const char __FAR Msg6912[] = {
 0
/* out of memory  */
,3,50,6,156
,0
/* attempt to deallocate an unallocated array or character*(*) variable  */
,9,42,7,136,64,176,65,10,67,56
/* attempt to allocate an already allocated array or character*(*) variable  */
,10,42,7,123,64,63,124,65,10,67,56
};
static const char __FAR Msg7168[] = {
 0
/* missing or misplaced opening parenthesis  */
,3,9,10,157
,0
/* unmatched parentheses  */
,1,178
};
static const char __FAR Msg9216[] = {
 0
,0
/* subscript expression out of range; %s1 does not exist  */
,9,84,46,50,6,161,26,27,0,72
/* substring expression (%i1:%i2) is out of range  */
,7,85,46,91,1,50,6,52
};
static const char __FAR Msg9728[] = {
 0
,0
/* invalid subscript expression  */
,3,2,84,46
,0
,0
,0
,0
,0
/* limit of 65535 elements per dimension has been exceeded  */
,4,153,6,95,45
};
static const char __FAR Msg11008[] = {
 0
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
/* - Executing line %u1 in file %s2  */
,6,38,107,74,5,8,58
/* - Called from line %u1 in file %s2  */
,7,38,103,73,74,5,8,58
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
};


//extern const char __FAR * const __FAR GroupTable[] = {
const char __FAR * const __FAR GroupTable[] = {
     NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,Msg1536
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,Msg3840
    ,NULL
    ,Msg4352
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,Msg5888
    ,Msg6144
    ,Msg6400
    ,NULL
    ,Msg6912
    ,Msg7168
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,Msg9216
    ,NULL
    ,Msg9728
    ,NULL
    ,NULL
    ,NULL
    ,NULL
    ,Msg11008
    };


//extern const char __FAR ErrWord[] = {
const char __FAR ErrWord[] = {
          3,'n','o','t'
/* count= 24 Group:offset = EX: 0, EX: 1, EX: 2, FM: 2, FM: 7,... */
          ,2,'i','s'
/* count= 20 Group:offset = EX: 0, EX: 1, EX: 2, FM: 2, FM: 7,... */
          ,7,'i','n','v','a','l','i','d'
/* count= 17 Group:offset = FM: 1, FM: 4, FM: 6, FM: 8, FM:13,... */
          ,9,'s','p','e','c','i','f','i','e','r'
/* count= 15 Group:offset = IO: 7, IO:12, IO:13, IO:16, IO:17,... */
          ,6,'f','o','r','m','a','t'
/* count= 12 Group:offset = FM: 0, FM: 2, FM: 4, FM: 5, FM: 7,... */
          ,2,'i','n'
/* count= 12 Group:offset = FM: 0, FM: 4, FM: 5, IO: 6, IO:13,... */
          ,2,'o','f'
/* count= 10 Group:offset = EX: 1, IO:10, IO:15, LI: 1, LI: 4,... */
          ,2,'t','o'
/* count=  8 Group:offset = IO: 7, IO:15, IO:36, IO:37, LI: 1,... */
          ,4,'f','i','l','e'
/* count=  7 Group:offset = IO: 8, IO:11, IO:15, IO:20, IO:29,... */
          ,7,'m','i','s','s','i','n','g'
/* count=  7 Group:offset = FM: 0, FM: 1, FM: 3, FM: 4, FM:18,... */
          ,2,'o','r'
/* count=  7 Group:offset = FM: 1, FM: 4, FM: 7, IO:26, MO: 3,... */
          ,7,'a','l','l','o','w','e','d'
/* count=  6 Group:offset = EX: 0, EX: 1, EX: 2, FM:10, FM:17,... */
          ,4,'c','o','d','e'
/* count=  6 Group:offset = FM: 2, FM: 7, FM:10, FM:15, FM:20,... */
          ,4,'e','d','i','t'
/* count=  6 Group:offset = FM: 4, FM: 5, FM:12, FM:18, FM:19,... */
          ,12,'i','n','p','u','t','/','o','u','t'
              ,'p','u','t'
/* count=  6 Group:offset = IO: 1, IO: 3, IO: 4, IO:17, IO:18,... */
          ,8,'r','e','q','u','i','r','e','s'
/* count=  6 Group:offset = IO: 0, IO: 2, IO: 3, IO: 4, IO: 5,... */
          ,9,'s','t','a','t','e','m','e','n','t'
/* count=  6 Group:offset = FM:16, IO: 0, IO: 2, IO: 5, IO:13,... */
          ,19,'F','O','R','T','R','A','N',' ','7'
              ,'7',' ','s','t','a','n','d','a','r','d'
/* count=  5 Group:offset = FM: 2, FM: 7, FM:15, FM:20, FM:21 */
          ,6,'a','c','c','e','s','s'
/* count=  5 Group:offset = IO: 0, IO: 2, IO: 5, IO:17, IO:18 */
          ,8,'a','r','g','u','m','e','n','t'
/* count=  5 Group:offset = LI: 0, LI: 1, LI: 2, LI: 4, LI: 8 */
          ,2,'b','e'
/* count=  5 Group:offset = IO: 7, LI: 0, LI: 2, LI: 3, LI: 4 */
          ,4,'d','a','t','a'
/* count=  5 Group:offset = IO: 9, IO:10, IO:15, IO:36, IO:37 */
          ,10,'d','e','s','c','r','i','p','t','o'
              ,'r'
/* count=  5 Group:offset = FM: 4, FM: 5, FM:12, FM:18, IO:26 */
          ,4,'m','u','s','t'
/* count=  5 Group:offset = LI: 0, LI: 1, LI: 2, LI: 3, LI: 4 */
          ,13,'s','p','e','c','i','f','i','c','a'
              ,'t','i','o','n'
/* count=  5 Group:offset = FM: 6, FM:10, FM:14, FM:17, IO: 9 */
          ,4,'z','e','r','o'
/* count=  5 Group:offset = KO: 0, KO: 3, LI: 0, LI: 3, LI: 4 */
          ,3,'%','s','1'
/* count=  4 Group:offset = IO: 8, IO:19, IO:30, SS: 2 */
          ,4,'d','o','e','s'
/* count=  4 Group:offset = IO: 9, IO:10, IO:23, SS: 2 */
          ,6,'d','u','r','i','n','g'
/* count=  4 Group:offset = IO:22, IO:31, IO:32, IO:33 */
          ,5,'e','r','r','o','r'
/* count=  4 Group:offset = IO: 8, IO:31, IO:32, IO:33 */
          ,5,'f','i','e','l','d'
/* count=  4 Group:offset = FM: 8, FM:13, FM:14, IO: 6 */
          ,14,'f','l','o','a','t','i','n','g','-'
              ,'p','o','i','n','t'
/* count=  4 Group:offset = IO:37, KO: 0, KO: 1, KO: 2 */
          ,3,'f','o','r'
/* count=  4 Group:offset = FM:10, IO:20, IO:26, IO:35 */
          ,5,'i','n','p','u','t'
/* count=  4 Group:offset = FM:17, IO: 6, IO:10, IO:31 */
          ,7,'i','n','t','e','g','e','r'
/* count=  4 Group:offset = IO:36, KO: 3, KO: 5 */
          ,6,'r','e','c','o','r','d'
/* count=  4 Group:offset = IO:22, IO:26, IO:35 */
          ,10,'s','e','q','u','e','n','t','i','a'
              ,'l'
/* count=  4 Group:offset = IO: 0, IO: 2, IO: 5, IO:18 */
          ,10,'s','t','a','t','e','m','e','n','t'
              ,'s'
/* count=  4 Group:offset = IO: 3, IO: 4, IO:17, IO:18 */
          ,1,'-'
/* count=  3 Group:offset = IO: 8, MS:128, MS:129 */
          ,17,'N','A','M','E','L','I','S','T','-'
              ,'d','i','r','e','c','t','e','d'
/* count=  3 Group:offset = IO:31, IO:32, IO:33 */
          ,3,'R','E','C'
/* count=  3 Group:offset = IO:17, IO:18, IO:24 */
          ,6,'S','T','A','T','U','S'
/* count=  3 Group:offset = IO:13, IO:20, IO:21 */
          ,7,'a','t','t','e','m','p','t'
/* count=  3 Group:offset = IO:15, MO: 3, MO: 4 */
          ,9,'c','h','a','r','a','c','t','e','r'
/* count=  3 Group:offset = IO: 6, IO:36, IO:37 */
          ,11,'c','o','m','b','i','n','a','t','i'
              ,'o','n'
/* count=  3 Group:offset = IO:16, IO:21, IO:28 */
          ,8,'e','x','c','e','e','d','e','d'
/* count=  3 Group:offset = IO:22, IO:37, SV: 8 */
          ,10,'e','x','p','r','e','s','s','i','o'
              ,'n'
/* count=  3 Group:offset = SS: 2, SS: 3, SV: 2 */
          ,9,'f','o','r','m','a','t','t','e','d'
/* count=  3 Group:offset = IO: 3, IO:26 */
          ,12,'g','r','e','a','t','e','r',' ','t'
              ,'h','a','n'
/* count=  3 Group:offset = FM:13, LI: 0, LI: 8 */
          ,3,'i','/','o'
/* count=  3 Group:offset = IO:29, IO:32, IO:33 */
          ,3,'o','u','t'
/* count=  3 Group:offset = MO: 1, SS: 2, SS: 3 */
          ,8,'o','v','e','r','f','l','o','w'
/* count=  3 Group:offset = IO:36, KO: 1, KO: 5 */
          ,5,'r','a','n','g','e'
/* count=  3 Group:offset = IO:37, LI: 4, SS: 3 */
          ,4,'s','i','z','e'
/* count=  3 Group:offset = FM:16, IO:22, IO:26 */
          ,4,'t','y','p','e'
/* count=  3 Group:offset = EX: 1, IO: 9, IO:10 */
          ,11,'u','n','f','o','r','m','a','t','t'
              ,'e','d'
/* count=  3 Group:offset = IO: 4, IO:22 */
          ,8,'v','a','r','i','a','b','l','e'
/* count=  3 Group:offset = IO:10, MO: 3, MO: 4 */
          ,5,'w','h','e','r','e'
/* count=  3 Group:offset = EX: 0, EX: 1, EX: 2 */
          ,3,'%','s','2'
/* count=  2 Group:offset = MS:128, MS:129 */
          ,6,'F','O','R','M','A','T'
/* count=  2 Group:offset = FM:16, FM:18 */
          ,4,'R','E','C','L'
/* count=  2 Group:offset = IO:12, IO:16 */
          ,1,'X'
/* count=  2 Group:offset = EX: 1, FM:19 */
          ,1,'Y'
/* count=  2 Group:offset = EX: 1, EX: 2 */
          ,7,'a','l','r','e','a','d','y'
/* count=  2 Group:offset = IO: 1, MO: 4 */
          ,2,'a','n'
/* count=  2 Group:offset = MO: 3, MO: 4 */
          ,5,'a','r','r','a','y'
/* count=  2 Group:offset = MO: 3, MO: 4 */
          ,7,'a','s','s','u','m','e','d'
/* count=  2 Group:offset = FM: 0, FM:19 */
          ,13,'c','h','a','r','a','c','t','e','r'
              ,'*','(','*',')'
/* count=  2 Group:offset = MO: 3, MO: 4 */
          ,10,'c','o','n','n','e','c','t','i','o'
              ,'n'
/* count=  2 Group:offset = IO: 3, IO: 4 */
          ,8,'c','o','n','s','t','a','n','t'
/* count=  2 Group:offset = FM: 1, FM:19 */
          ,10,'c','o','n','v','e','r','t','i','n'
              ,'g'
/* count=  2 Group:offset = IO:36, IO:37 */
          ,9,'d','i','v','i','d','e',' ','b','y'
/* count=  2 Group:offset = KO: 0, KO: 3 */
          ,5,'e','x','i','s','t'
/* count=  2 Group:offset = IO:23, SS: 2 */
          ,4,'f','r','o','m'
/* count=  2 Group:offset = KO: 4, MS:129 */
          ,8,'l','i','n','e',' ','%','u','1'
/* count=  2 Group:offset = MS:128, MS:129 */
          ,5,'m','a','t','c','h'
/* count=  2 Group:offset = IO: 9, IO:10 */
          ,4,'m','o','d','e'
/* count=  2 Group:offset = IO: 0, IO: 2 */
          ,2,'n','o'
/* count=  2 Group:offset = FM:12, IO:35 */
          ,2,'o','n'
/* count=  2 Group:offset = FM:12, FM:17 */
          ,3,'o','n','e'
/* count=  2 Group:offset = LI: 1, LI: 8 */
          ,9,'o','p','e','r','a','t','i','o','n'
/* count=  2 Group:offset = IO:29, IO:35 */
          ,7,'p','r','o','g','r','a','m'
/* count=  2 Group:offset = CP: 0, KO: 4 */
          ,6,'r','e','p','e','a','t'
/* count=  2 Group:offset = FM: 6, FM:10 */
          ,6,'s','t','r','i','n','g'
/* count=  2 Group:offset = FM: 4, FM: 5 */
          ,9,'s','u','b','s','c','r','i','p','t'
/* count=  2 Group:offset = SS: 2, SV: 2 */
          ,9,'s','u','b','s','t','r','i','n','g'
/* count=  2 Group:offset = IO:33, SS: 3 */
          ,3,'t','h','e'
/* count=  2 Group:offset = IO:10, LI: 4 */
          ,14,'u','n','i','t',' ','s','p','e','c'
              ,'i','f','i','e','d'
/* count=  2 Group:offset = IO:14, IO:23 */
          ,4,'w','i','t','h'
/* count=  2 Group:offset = IO:29, IO:35 */
          ,1,'$'
/* count=  1 Group:offset = FM: 7 */
          ,11,'\'','F','O','R','M','A','T','T','E'
              ,'D','\''
/* count=  1 Group:offset = IO: 7 */
          ,9,'(','%','i','1',':','%','i','2',')'
/* count=  1 Group:offset = SS: 3 */
          ,8,'(','0',',','0',')','*','*','Y'
/* count=  1 Group:offset = EX: 2 */
          ,3,'2','5','5'
/* count=  1 Group:offset = LI: 4 */
          ,3,'2','5','6'
/* count=  1 Group:offset = FM:13 */
          ,37,'6','5','5','3','5',' ','e','l','e'
              ,'m','e','n','t','s',' ','p','e','r',' '
              ,'d','i','m','e','n','s','i','o','n',' '
              ,'h','a','s',' ','b','e','e','n'
/* count=  1 Group:offset = SV: 8 */
          ,6,'<',' ','0','.','0',','
/* count=  1 Group:offset = EX: 1 */
          ,9,'B','A','C','K','S','P','A','C','E'
/* count=  1 Group:offset = IO: 0 */
          ,5,'B','L','A','N','K'
/* count=  1 Group:offset = IO: 7 */
          ,9,'B','L','O','C','K','S','I','Z','E'
/* count=  1 Group:offset = IO:34 */
          ,30,'C','A','R','R','I','A','G','E','C'
              ,'O','N','T','R','O','L',' ','s','p','e'
              ,'c','i','f','i','e','r','/','F','O','R'
              ,'M'
/* count=  1 Group:offset = IO:28 */
          ,4,'C','H','A','R'
/* count=  1 Group:offset = LI: 4 */
          ,5,'C','L','O','S','E'
/* count=  1 Group:offset = IO:13 */
          ,6,'C','a','l','l','e','d'
/* count=  1 Group:offset = MS:129 */
          ,7,'E','N','D','F','I','L','E'
/* count=  1 Group:offset = IO: 2 */
          ,6,'E','w','.','d','D','e'
/* count=  1 Group:offset = FM: 2 */
          ,6,'E','w','.','d','Q','e'
/* count=  1 Group:offset = FM:20 */
          ,9,'E','x','e','c','u','t','i','n','g'
/* count=  1 Group:offset = MS:128 */
          ,4,'F','O','R','M'
/* count=  1 Group:offset = IO: 7 */
          ,5,'I','C','H','A','R'
/* count=  1 Group:offset = LI: 8 */
          ,8,'I','N','T','E','G','E','R',','
/* count=  1 Group:offset = EX: 1 */
          ,6,'J',' ','<','=',' ','0'
/* count=  1 Group:offset = EX: 0 */
          ,8,'N','A','M','E','L','I','S','T'
/* count=  1 Group:offset = IO:30 */
          ,4,'Q','w','.','d'
/* count=  1 Group:offset = FM:21 */
          ,6,'R','E','W','I','N','D'
/* count=  1 Group:offset = IO: 5 */
          ,4,'U','N','I','T'
/* count=  1 Group:offset = IO:25 */
          ,4,'X','*','*','Y'
/* count=  1 Group:offset = EX: 1 */
          ,1,'Z'
/* count=  1 Group:offset = FM:15 */
          ,1,'\\'
/* count=  1 Group:offset = FM: 7 */
          ,17,'a',' ','s','u','b','s','e','q','u'
              ,'e','n','t',' ','O','P','E','N'
/* count=  1 Group:offset = IO:19 */
          ,21,'a','b','n','o','r','m','a','l','l'
              ,'y',' ','t','e','r','m','i','n','a','t'
              ,'e','d'
/* count=  1 Group:offset = CP: 0 */
          ,14,'a','b','s','o','l','u','t','e',' '
              ,'v','a','l','u','e'
/* count=  1 Group:offset = LI: 1 */
          ,6,'a','c','t','i','v','e'
/* count=  1 Group:offset = IO: 1 */
          ,8,'a','l','l','o','c','a','t','e'
/* count=  1 Group:offset = MO: 4 */
          ,9,'a','l','l','o','c','a','t','e','d'
/* count=  1 Group:offset = MO: 4 */
          ,10,'a','p','o','s','t','r','o','p','h'
              ,'e'
/* count=  1 Group:offset = FM:12 */
          ,18,'a','r','c','s','i','n','e',',',' '
              ,'a','r','c','c','o','s','i','n','e'
/* count=  1 Group:offset = LI: 1 */
          ,11,'a','r','g','u','m','e','n','t','('
              ,'s',')'
/* count=  1 Group:offset = LI: 3 */
          ,10,'a','t','t','r','i','b','u','t','e'
              ,'s'
/* count=  1 Group:offset = IO:29 */
          ,3,'b','a','d'
/* count=  1 Group:offset = IO: 6 */
          ,6,'b','e','f','o','r','e'
/* count=  1 Group:offset = FM:19 */
          ,6,'c','h','a','n','g','e'
/* count=  1 Group:offset = IO:19 */
          ,13,'c','l','o','s','i','n','g',' ','q'
              ,'u','o','t','e'
/* count=  1 Group:offset = FM:12 */
          ,9,'c','o','n','n','e','c','t','e','d'
/* count=  1 Group:offset = IO:14 */
          ,10,'c','o','n','s','i','s','t','e','n'
              ,'t'
/* count=  1 Group:offset = IO:29 */
          ,5,'c','o','u','n','t'
/* count=  1 Group:offset = FM:13 */
          ,10,'d','e','a','l','l','o','c','a','t'
              ,'e'
/* count=  1 Group:offset = MO: 3 */
          ,13,'d','e','c','i','m','a','l',' ','p'
              ,'o','i','n','t'
/* count=  1 Group:offset = FM: 3 */
          ,9,'d','e','l','i','m','i','t','e','r'
/* count=  1 Group:offset = FM: 0 */
          ,13,'d','e','s','c','r','i','p','t','o'
              ,'r',',',' ','1'
/* count=  1 Group:offset = FM:19 */
          ,6,'d','i','r','e','c','t'
/* count=  1 Group:offset = IO:17 */
          ,6,'e','x','c','e','e','d'
/* count=  1 Group:offset = LI: 1 */
          ,24,'e','x','c','e','e','d','s',' ','a'
              ,'l','l','o','t','t','e','d',' ','s','t'
              ,'o','r','a','g','e'
/* count=  1 Group:offset = FM:16 */
          ,5,'f','i','l','e','s'
/* count=  1 Group:offset = IO:35 */
          ,5,'f','o','u','n','d'
/* count=  1 Group:offset = IO:30 */
          ,4,'f','u','l','l'
/* count=  1 Group:offset = IO:11 */
          ,5,'g','i','v','e','n'
/* count=  1 Group:offset = IO:20 */
          ,14,'i','l','l','e','g','a','l',' ','\''
              ,'%','s','1','=','\''
/* count=  1 Group:offset = IO:27 */
          ,8,'i','n','t','e','r','n','a','l'
/* count=  1 Group:offset = IO:11 */
          ,11,'i','n','t','e','r','r','u','p','t'
              ,'e','d'
/* count=  1 Group:offset = KO: 4 */
          ,4,'i','t','e','m'
/* count=  1 Group:offset = IO:10 */
          ,8,'k','e','y','b','o','a','r','d'
/* count=  1 Group:offset = KO: 4 */
          ,6,'l','e','n','g','t','h'
/* count=  1 Group:offset = LI: 8 */
          ,5,'l','i','m','i','t'
/* count=  1 Group:offset = SV: 8 */
          ,4,'l','i','s','t'
/* count=  1 Group:offset = IO:10 */
          ,3,'m','a','y'
/* count=  1 Group:offset = IO:19 */
          ,6,'m','e','m','o','r','y'
/* count=  1 Group:offset = MO: 1 */
          ,29,'m','i','s','p','l','a','c','e','d'
              ,' ','o','p','e','n','i','n','g',' ','p'
              ,'a','r','e','n','t','h','e','s','i','s'
/* count=  1 Group:offset = PC: 1 */
          ,8,'m','o','d','i','f','i','e','r'
/* count=  1 Group:offset = FM: 8 */
          ,8,'n','e','g','a','t','i','v','e'
/* count=  1 Group:offset = LI: 2 */
          ,7,'p','e','r','f','o','r','m'
/* count=  1 Group:offset = IO:15 */
          ,6,'r','a','n','g','e',';'
/* count=  1 Group:offset = SS: 2 */
          ,4,'r','e','a','l'
/* count=  1 Group:offset = EX: 2 */
          ,10,'r','e','p','e','a','t','a','b','l'
              ,'e'
/* count=  1 Group:offset = FM:18 */
          ,8,'r','e','q','u','i','r','e','d'
/* count=  1 Group:offset = IO:17 */
          ,16,'s','p','e','c','i','f','i','e','r'
              ,'/','A','C','C','E','S','S'
/* count=  1 Group:offset = IO:16 */
          ,14,'s','p','e','c','i','f','i','e','r'
              ,'/','F','I','L','E'
/* count=  1 Group:offset = IO:21 */
          ,13,'s','t','r','i','n','g',',',' ','c'
              ,'o','m','m','a'
/* count=  1 Group:offset = FM: 0 */
          ,9,'s','t','r','u','c','t','u','r','e'
/* count=  1 Group:offset = IO:35 */
          ,12,'s','u','b','s','c','r','i','p','t'
              ,'i','n','g'
/* count=  1 Group:offset = IO:32 */
          ,6,'s','y','m','b','o','l'
/* count=  1 Group:offset = IO:30 */
          ,6,'s','y','n','t','a','x'
/* count=  1 Group:offset = IO:31 */
          ,6,'s','y','s','t','e','m'
/* count=  1 Group:offset = IO: 8 */
          ,4,'t','h','i','s'
/* count=  1 Group:offset = FM:10 */
          ,9,'t','o','o',' ','l','a','r','g','e'
/* count=  1 Group:offset = IO:26 */
          ,17,'t','r','a','n','s','f','e','r',' '
              ,'p','a','s','t',' ','e','n','d'
/* count=  1 Group:offset = IO:15 */
          ,11,'u','n','a','l','l','o','c','a','t'
              ,'e','d'
/* count=  1 Group:offset = MO: 3 */
          ,9,'u','n','d','e','r','f','l','o','w'
/* count=  1 Group:offset = KO: 2 */
          ,21,'u','n','m','a','t','c','h','e','d'
              ,' ','p','a','r','e','n','t','h','e','s'
              ,'e','s'
/* count=  1 Group:offset = PC: 3 */
          ,14,'u','n','r','e','c','o','g','n','i'
              ,'z','a','b','l','e'
/* count=  1 Group:offset = FM: 5 */
          ,5,'w','i','d','t','h'
/* count=  1 Group:offset = FM:14 */
          ,7,'z','e','r','o','*','*','J'
/* count=  1 Group:offset = EX: 0 */
                   };
/* Total number of phrases = 182 */
