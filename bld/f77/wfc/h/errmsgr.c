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

static const char __FAR Msg0[] = {
/* invalid number of arguments to intrinsic function %s1  */
 8,20,26,5,92,8,37,12,0
/* dummy argument %s1 appears more than once  */
,7,73,17,0,69,100,77,86
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* dummy argument %s1 must not appear before definition of ENTRY %s2  */
,11,73,17,0,14,1,79,111,53,5,198,11
};
static const char __FAR Msg256[] = {
/* %s1 was initialized in a block data subprogram but is not in COMMON  */
 13,0,62,121,3,9,29,63,50,45,4,1,3,22
/* %s1 statement is not permitted in a BLOCK DATA subprogram  */
,11,0,2,4,1,255,234,3,9,67,142,50
};
static const char __FAR Msg512[] = {
/* invalid character encountered in source input  */
 6,20,23,228,3,255,22
/* invalid character in statement number columns  */
,6,20,23,3,2,26,217
/* character in continuation column, but no statement to continue  */
,11,23,3,94,255,139,45,76,2,8,255,144
/* character encountered is not FORTRAN 77 standard  */
,5,23,228,4,1,6
/* columns 1-5 in a continuation line must be blank  */
,10,217,255,51,3,9,94,30,14,7,147
/* more than 19 continuation lines is not FORTRAN 77 standard  */
,10,100,77,255,52,94,255,0,4,1,6
/* end-of-line comment is not FORTRAN 77 standard  */
,6,255,160,218,4,1,6
/* D in column 1 is not FORTRAN 77 standard  */
,8,255,68,3,152,138,4,1,6
/* too many continuation lines  */
,5,104,172,94,255,0
};
static const char __FAR Msg768[] = {
/* %s1 already in COMMON  */
 4,0,57,3,22
/* initializing %s1 in COMMON outside of block data subprogram is not FORTRAN 77 standard  */
,14,255,194,0,3,22,255,13,5,29,63,50,4,1,6
/* character and non-character data in COMMON is not FORTRAN 77 standard  */
,10,23,51,255,9,63,3,22,4,1,6
/* COMMON block %s1 has been defined with a different size  */
,10,22,29,0,24,39,52,28,9,157,40
/* named COMMON block %s1 appears in more than one BLOCK DATA subprogram  */
,13,255,220,22,29,0,69,3,100,77,125,67,142,50
/* blank COMMON block has been defined with a different size  */
,10,147,22,29,24,39,52,28,9,157,40
};
static const char __FAR Msg1024[] = {
/* DOUBLE PRECISION COMPLEX constants are not FORTRAN 77 standard  */
 6,106,196,115,80,1,6
/* invalid floating-point constant %s1  */
,4,20,118,34,0
/* zero length character constants are not allowed  */
,7,105,47,23,115,80,1,19
/* invalid hexadecimal/octal constant  */
,3,20,242,34
/* hexadecimal/octal constant is not FORTRAN 77 standard  */
,5,242,34,4,1,6
};
static const char __FAR Msg1280[] = {
/* %s1 is already being included  */
 5,0,4,57,255,125
/* '%s1' option cannot take a NO prefix  */
,9,78,101,16,255,255,22,9,255,91
/* expecting an equals sign following the %s1 option  */
,9,10,15,231,181,255,177,27,0,101
/* the '%s1' option requires a number  */
,6,27,78,101,129,9,26
/* option '%s1' not recognized - ignored  */
,7,101,78,1,255,244,56,246
/* '%s1' option not allowed in source input stream  */
,10,78,101,1,19,3,255,22,255,255,13
/* nesting level exceeded for compiler directives  */
,8,255,8,255,206,161,35,114,225
/* mismatching compiler directives  */
,4,255,213,114,225
};
static const char __FAR Msg1536[] = {
 0
/* argument %d1 incompatible with register  */
,6,17,136,255,191,28,128
/* subprogram %s1 has invalid return register  */
,6,50,0,24,20,130,128
/* low on memory - unable to fully optimize %s1  */
,14,255,4,85,99,56,255,255,32,8,255,185,255,10,0
/* internal compiler error %d1  */
,4,122,114,81,136
/* illegal register modified by %s1  */
,6,36,128,255,216,112,0
/* %s1  */
,1,0
/* fatal: %s1  */
,3,255,173,0
/* dynamic memory not freed  */
,5,116,99,1,255,182
/* freeing unowned dynamic memory  */
,4,255,183,116,99
/* The automatic equivalence containing %s1 exceeds 32K limit  */
,9,143,212,255,162,220,0,96,140,84
/* The return value of %s1 exceeds 32K limit  */
,8,143,130,188,5,0,96,140,84
/* The automatic variable %s1 exceeds 32K limit  */
,7,143,212,21,0,96,140,84
};
static const char __FAR Msg1792[] = {
/* CHARACTER variable %s1 with length (*) not allowed in this expression  */
 11,141,21,0,28,47,89,1,19,3,55,64
/* character variable %s1 with length (*) must be a subprogram argument  */
,11,23,21,0,28,47,89,14,7,9,50,17
};
static const char __FAR Msg2048[] = {
/* implied DO variable %s1 must be an integer variable  */
 9,163,43,21,0,14,7,15,65,21
/* repeat specification must be a positive integer  */
,8,179,49,14,7,9,255,236,65
/* %s1 appears in an expression but is not an implied DO variable  */
,12,0,69,3,15,64,45,4,1,15,163,43,21
/* %s1 in blank COMMON block cannot be initialized  */
,8,0,3,147,22,29,16,7,121
/* data initialization with hexadecimal constant is not FORTRAN 77 standard  */
,8,63,250,28,241,34,4,1,6
/* cannot initialize %s1 %s2  */
,4,16,251,0,11
/* data initialization in %s1 statement is not FORTRAN 77 standard  */
,8,63,250,3,0,2,4,1,6
/* not enough constants for list of variables  */
,7,1,229,115,35,48,5,88
,0
/* too many constants for list of variables  */
,7,104,172,115,35,48,5,88
,0
/* cannot initialize %s1 variable %s2 with %s3 constant  */
,8,16,251,0,21,11,28,137,34
};
static const char __FAR Msg2304[] = {
/* using %s1 incorrectly in dimension expression  */
 6,187,0,165,3,224,64
};
static const char __FAR Msg2560[] = {
/* statement number %i1 already defined in line %d2 - DO loop is backwards  */
 14,2,26,42,57,52,3,30,41,56,43,54,4,255,123
/* %s1 statement not allowed at termination of DO range  */
,11,0,2,1,19,70,255,255,27,5,43,103
/* improper nesting of DO loop  */
,6,248,255,8,5,43,54
/* ENDDO cannot terminate DO loop with statement label  */
,11,255,74,16,255,255,26,43,54,28,2,168
/* this DO loop form is not FORTRAN 77 standard  */
,8,55,43,54,255,178,4,1,6
/* expecting comma or DO variable  */
,5,10,113,13,43,21
/* DO variable cannot be redefined while DO loop is active  */
,11,43,21,16,7,255,246,43,54,4,255,105
};
static const char __FAR Msg2816[] = {
/* equivalencing %s1 has caused extension of COMMON block %s2 to the left  */
 14,232,0,24,255,137,236,5,22,29,11,8,27,255,203
/* %s1 and %s2 in COMMON are equivalenced to each other  */
,12,0,51,11,3,22,80,160,8,255,159,255,11
};
static const char __FAR Msg3072[] = {
/* missing END statement  */
 3,25,197,2
};
static const char __FAR Msg3328[] = {
/* target of assignment is illegal  */
 7,255,255,23,5,145,4,36
/* cannot assign value to %s1  */
,6,16,255,120,188,8,0
/* illegal use of equal sign  */
,6,36,134,5,255,161,181
/* multiple assignment is not FORTRAN 77 standard  */
,6,255,6,145,4,1,6
/* expecting equals sign  */
,3,10,231,181
};
static const char __FAR Msg3584[] = {
/* %s1 has been equivalenced to 2 different relative positions  */
 9,0,24,39,160,8,139,157,255,248
/* EQUIVALENCE list must contain at least 2 names  */
,9,199,48,14,153,70,169,139,255,7
/* %s1 incorrectly subscripted in %s2 statement  */
,8,0,165,255,255,18,3,11,2
/* incorrect substring of %s1 in %s2 statement  */
,7,249,183,5,0,3,11,2
/* equivalencing CHARACTER and non-CHARACTER data is not FORTRAN 77 standard  */
,9,232,141,51,255,222,63,4,1,6
/* attempt to substring %s1 in EQUIVALENCE statement but type is %s2  */
,11,110,8,183,0,3,199,2,45,33,4,11
};
static const char __FAR Msg3840[] = {
/* zero**J where J <= 0 is not allowed  */
 6,255,255,50,4,1,19
};
static const char __FAR Msg4096[] = {
/* type of entry %s1 does not match type of function %s2  */
 11,33,5,230,0,158,1,173,33,5,12,11
/* ENTRY statement not allowed within structured control blocks  */
,10,198,2,1,19,135,255,255,15,72,213
/* size of entry %s1 does not match size of function %s2  */
,11,40,5,230,0,158,1,173,40,5,12,11
};
static const char __FAR Msg4352[] = {
/* missing delimiter in format string, comma assumed  */
 10,25,255,148,3,46,255,255,14,113,109
/* missing or invalid constant  */
,4,25,13,20,34
/* Ew.dDe format code is not FORTRAN 77 standard  */
,7,255,76,46,71,4,1,6
/* missing decimal point  */
,3,25,255,146
/* missing or invalid edit descriptor in format string  */
,8,25,13,20,117,156,3,46,182
/* unrecognizable edit descriptor in format string  */
,6,186,117,156,3,46,182
/* invalid repeat specification  */
,3,20,179,49
/* $ or \ format code is not FORTRAN 77 standard  */
,10,255,34,13,255,104,46,71,4,1,6
/* invalid field modifier  */
,4,20,82,255,217
/* expecting end of FORMAT statement but found more text  */
,11,10,74,5,90,2,45,98,100,255,255,28
/* repeat specification not allowed for this format code  */
,8,179,49,1,19,35,55,46,71
/* no statement number on FORMAT statement  */
,6,76,2,26,85,90,2
/* no closing quote on apostrophe edit descriptor  */
,9,76,151,255,16,85,255,114,117,156
/* field count greater than 256 is invalid  */
,8,82,255,145,77,255,55,4,20
/* invalid field width specification  */
,6,20,82,255,255,45,49
/* Z format code is not FORTRAN 77 standard  */
,7,255,103,46,71,4,1,6
,0
,0
,0
/* missing constant before X edit descriptor, 1 assumed  */
,10,25,34,111,255,102,117,255,150,138,109
/* Ew.dQe format code is not FORTRAN 77 standard  */
,7,255,77,46,71,4,1,6
/* Qw.d format code is not FORTRAN 77 standard  */
,7,255,93,46,71,4,1,6
};
static const char __FAR Msg4608[] = {
/* %s1 statement label may not appear in ASSIGN statement but did in line %d2  */
 15,0,2,168,174,1,79,3,193,2,45,255,153,3,30,41
/* ASSIGN of statement number %i1 in line %d2 not allowed  */
,10,193,5,2,26,42,3,30,41,1,19
/* expecting TO  */
,3,10,255,98
};
static const char __FAR Msg4864[] = {
/* hollerith constant is not FORTRAN 77 standard  */
 5,243,34,4,1,6
/* not enough characters for hollerith constant  */
,6,1,229,149,35,243,34
};
static const char __FAR Msg5120[] = {
/* ELSE block must be the last block in block IF  */
 13,255,72,29,14,7,27,255,202,29,3,29,255,81
/* expecting THEN  */
,3,10,255,97
};
static const char __FAR Msg5376[] = {
/* missing or invalid format/FMT specification  */
 6,25,13,20,255,179,49
/* the UNIT may not be an internal file for this statement  */
,11,27,207,174,1,7,15,122,31,35,55,2
/* %s1 statement cannot have %s2 specification  */
,6,0,2,16,120,11,49
/* variable must have a size of 4  */
,8,21,14,120,9,40,5,255,56
/* missing or unrecognizable control list item %s1  */
,7,25,13,186,72,48,167,0
/* attempt to specify control list item %s1 more than once  */
,12,110,8,255,255,9,72,48,167,0,100,77,86
/* implied DO loop has no input/output list  */
,7,163,43,54,24,76,252,48
/* list-directed input/output with internal files is not FORTRAN 77 standard  */
,9,255,1,252,28,122,97,4,1,6
/* FORTRAN 77 standard requires an asterisk for list-directed formatting  */
,9,6,129,15,146,35,255,1,255,181
/* missing or improper unit identification  */
,5,25,13,248,185,244
/* missing unit identification or file specification  */
,6,25,185,244,13,31,49
/* asterisk unit identifier not allowed in %s1 statement  */
,8,146,185,245,1,19,3,0,2
/* cannot have both UNIT and FILE specifier  */
,8,16,120,148,207,51,255,79,61
/* internal files require sequential access  */
,4,122,97,255,249
/* END specifier with REC specifier is not FORTRAN 77 standard  */
,9,197,61,28,255,95,61,4,1,6
/* %s1 specifier in i/o list is not FORTRAN 77 standard  */
,8,0,61,3,162,48,4,1,6
/* i/o list is not allowed with NAMELIST-directed format  */
,9,162,48,4,1,19,28,255,90,46
/* non-character array as format specifier is not FORTRAN 77 standard  */
,9,255,9,38,44,46,61,4,1,6
};
static const char __FAR Msg5632[] = {
/* illegal range of characters  */
 4,36,103,5,149
/* letter can only be implicitly declared once  */
,9,255,205,93,59,7,255,190,154,86
/* unrecognizable type  */
,2,186,33
/* (*) length specifier in an IMPLICIT statement is not FORTRAN 77 standard  */
,10,89,47,61,3,15,107,2,4,1,6
/* IMPLICIT NONE allowed once or not allowed with other IMPLICIT statements  */
,14,107,255,92,19,86,13,1,19,28,255,11,107,255,23
};
static const char __FAR Msg6144[] = {
/* floating-point divide by zero  */
 4,118,226,112,105
/* floating-point overflow  */
,2,118,177
/* floating-point underflow  */
,4,118,255,255,35
/* integer divide by zero  */
,4,65,226,112,105
/* program interrupted from keyboard  */
,6,102,255,197,119,255,200
/* integer overflow  */
,2,65,177
};
static const char __FAR Msg6400[] = {
 0
,0
,0
,0
/* argument of CHAR must be in the range zero to 255  */
,13,17,5,255,63,14,7,3,27,103,105,8,255,54
/* %s1 intrinsic function cannot be passed 2 complex arguments  */
,9,0,37,12,16,7,127,139,219,92
/* argument types must be the same for the %s1 intrinsic function  */
,13,17,255,28,14,7,27,255,20,35,27,0,37,12
/* expecting numeric argument, but %s1 argument was found  */
,9,10,255,223,210,45,0,17,62,98
,0
/* cannot pass %s1 as argument to intrinsic function  */
,8,16,178,0,44,17,8,37,12
/* intrinsic function requires argument(s)  */
,5,37,12,129,255,116
/* %s1 argument type is invalid for this generic function  */
,10,0,17,33,4,20,35,55,255,188,12
/* this intrinsic function cannot be passed as an argument  */
,9,55,37,12,16,7,127,44,15,17
/* expecting %s1 argument, but %s2 argument was found  */
,8,10,0,210,45,11,17,62,98
/* intrinsic function was assigned wrong type  */
,6,37,12,62,255,121,33
/* intrinsic function %s1 is not FORTRAN 77 standard  */
,6,37,12,0,4,1,6
/* argument to ALLOCATED intrinsic function must be an allocatable array or character*(*) variable  */
,15,17,8,255,62,37,12,14,7,15,208,38,13,255,138,21
/* invalid argument to ISIZEOF intrinsic function  */
,7,20,17,8,255,84,37,12
};
static const char __FAR Msg6656[] = {
/* relational operator has a logical operand  */
 8,255,247,60,24,9,170,255,226
/* mixing DOUBLE PRECISION and COMPLEX types is not FORTRAN 77 standard  */
,10,255,215,106,51,196,255,28,4,1,6
/* operator not expecting %s1 operands  */
,5,60,1,10,0,176
/* operator not expecting %s1 and %s2 operands  */
,7,60,1,10,0,51,11,176
/* complex quantities can only be compared using .EQ. or .NE.  */
,14,219,255,242,93,59,7,255,140,187,255,47,13,255,48
/* unary operator not expecting %s1 type  */
,8,255,255,33,60,1,10,0,33
/* logical operator with integer operands is not FORTRAN 77 standard  */
,8,170,60,28,65,176,4,1,6
/* logical operator %s1 is not FORTRAN 77 standard  */
,6,170,60,0,4,1,6
};
static const char __FAR Msg6912[] = {
/* %s1 exceeds compiler limit of %u2 bytes  */
 8,0,96,114,84,5,190,255,131
/* out of memory  */
,4,255,12,5,99
/* dynamic memory exhausted due to length of this statement - statement ignored  */
,13,116,99,255,166,159,8,47,5,55,2,56,2,246
};
static const char __FAR Msg7168[] = {
/* missing or misplaced closing parenthesis  */
 6,25,13,255,5,151,126
/* missing or misplaced opening parenthesis  */
,6,25,13,255,5,175,126
/* unexpected parenthesis  */
,2,184,126
/* unmatched parentheses  */
,3,255,255,39
};
static const char __FAR Msg7424[] = {
/* expecting symbolic name  */
 3,10,133,18
/* illegal size specified for VALUE attribute  */
,9,36,40,255,255,8,35,255,100,211
/* illegal argument attribute  */
,3,36,17,211
/* continuation line must contain a comment character in column 1  */
,10,94,30,14,153,9,218,23,3,152,138
/* expecting '%s1' near '%s2'  */
,4,10,78,255,221
/* in-line byte sequence limit exceeded  */
,5,164,214,180,84,161
/* illegal hexadecimal data in byte sequence  */
,6,36,241,63,3,214,180
/* symbol '%s1' in in-line assembly code cannot be resolved  */
,12,255,25,78,3,164,255,119,71,16,7,255,250
};
static const char __FAR Msg7680[] = {
/* alternate return specifier only allowed in subroutine  */
 8,255,112,130,61,59,19,3,132
/* RETURN statement in main program is not FORTRAN 77 standard  */
,8,205,2,3,171,102,4,1,6
};
static const char __FAR Msg7936[] = {
/* COMMON block %s1 saved but not properly defined  */
 10,22,29,0,255,21,45,1,255,241,52
/* COMMON block %s1 must be saved in every subprogram in which it appears  */
,16,22,29,0,14,7,255,21,3,255,165,50,3,255,255,44,69
/* name already appeared in a previous SAVE statement  */
,10,18,57,255,115,3,9,255,238,206,2
};
static const char __FAR Msg8192[] = {
/* statement function definition contains duplicate dummy arguments  */
 8,2,12,53,221,255,157,73,92
/* character length of statement function name must not be (*)  */
,10,23,47,5,2,12,18,14,1,7,89
/* statement function definition contains illegal dummy argument  */
,7,2,12,53,221,36,73,17
/* cannot pass %s1 %s2 to statement function  */
,7,16,178,0,11,8,2,12
/* %s1 actual argument was passed to %s2 dummy argument  */
,10,0,255,106,17,62,127,8,11,73,17
/* incorrect number of arguments passed to statement function %s1  */
,9,249,26,5,92,127,8,2,12,0
/* type of statement function name must not be a user-defined structure  */
,13,33,5,2,12,18,14,1,7,9,255,255,41,32
};
static const char __FAR Msg8448[] = {
/* system file error reading %s1 - %s2  */
 9,255,26,31,81,255,243,0,56,11
/* error opening file %s1 - %s2  */
,6,81,175,31,0,56,11
/* system file error writing %s1 - %s2  */
,10,255,26,31,81,255,255,49,0,56,11
,0
,0
/* error opening %s1 - too many temporary files exist  */
,12,81,175,0,56,104,172,255,255,24,97,255,167
/* generation of browsing information failed  */
,8,255,187,5,255,130,83,255,172
};
static const char __FAR Msg8704[] = {
/* cannot have both ATEND and the END= specifier  */
 9,16,120,148,194,51,27,255,73,61
/* ATEND must immediately follow a READ statement  */
,8,194,14,247,239,9,255,94,2
/* block label must be a symbolic name  */
,7,29,168,14,7,9,133,18
/* could not find a structure to %s1 from  */
,9,222,1,255,175,9,32,8,0,119
/* REMOTE BLOCK is not allowed in the range of any control structure  */
,13,91,67,4,1,19,3,27,103,5,255,113,72,32
/* the SELECT statement must be followed immediately by a CASE statement  */
,13,27,255,96,2,14,7,255,176,247,112,9,195,2
/* cases are overlapping  */
,5,255,136,80,255,231
/* select structure requires at least one CASE statement  */
,10,255,255,3,32,129,70,169,125,195,2
/* cannot branch to %i1 from outside control structure in line %d2  */
,12,16,58,8,42,119,255,13,72,32,3,30,41
/* cannot branch to %i1 inside structure on line %d2  */
,10,16,58,8,42,255,195,32,85,30,41
/* low end of range exceeds the high end  */
,10,255,4,74,5,103,96,27,255,189,74
/* default case block must follow all case blocks  */
,9,155,215,29,14,239,255,109,215,213
/* attempt to branch out of a REMOTE BLOCK  */
,9,110,8,58,255,12,5,9,91,67
/* attempt to EXECUTE undefined REMOTE BLOCK %s1  */
,7,110,8,255,75,91,67,0
/* attempted to use REMOTE BLOCK recursively  */
,8,255,122,8,134,91,67,255,245
/* cannot RETURN from subprogram within a REMOTE BLOCK  */
,8,16,205,119,50,135,9,91,67
/* %s1 statement is not FORTRAN 77 standard  */
,5,0,2,4,1,6
/* %s1 block is unfinished  */
,6,0,29,4,255,255,36
/* %s1 statement does not match with %s2 statement  */
,8,0,2,158,1,173,28,11,2
/* incomplete control structure found at %s1 statement  */
,8,255,192,72,32,98,70,0,2
/* %s1 statement is not allowed in %s2 definition  */
,8,0,2,4,1,19,3,11,53
/* no such field name found in structure %s1  */
,10,76,255,255,20,82,18,98,3,32,0
/* multiple definition of field name %s1  */
,7,255,6,53,5,82,18,0
/* structure %s1 has not been defined  */
,6,32,0,24,1,39,52
/* structure %s1 has already been defined  */
,6,32,0,24,57,39,52
/* structure %s1 must contain at least one field  */
,8,32,0,14,153,70,169,125,82
/* recursion detected in definition of structure %s1  */
,7,255,17,3,53,5,32,0
/* illegal use of structure %s1 containing union  */
,9,36,134,5,32,0,220,255,255,37
/* allocatable arrays cannot be fields within structures  */
,11,208,255,118,16,7,255,174,135,255,255,16
/* an integer conditional expression is not FORTRAN 77 standard  */
,8,15,65,255,143,64,4,1,6
/* %s1 statement must be used within %s2 definition  */
,8,0,2,14,7,87,135,11,53
};
static const char __FAR Msg8960[] = {
/* name can only appear in an EXTERNAL statement once  */
 9,18,93,59,79,3,15,200,2,86
/* character function %s1 may not be called since size was declared as (*)  */
,13,23,12,0,174,1,7,255,134,40,62,154,44,89
/* %s1 can only be used an an argument to a subroutine  */
,11,0,93,59,7,87,15,15,17,8,9,132
/* name cannot appear in both an INTRINSIC and EXTERNAL statement  */
,10,18,16,79,3,148,15,203,51,200,2
/* expecting a subroutine name  */
,4,10,9,132,18
/* %s1 statement not allowed in main program  */
,7,0,2,1,19,3,171,102
/* not an intrinsic FUNCTION name  */
,6,1,15,37,255,80,18
/* name can only appear in an INTRINSIC statement once  */
,9,18,93,59,79,3,15,203,2,86
/* subprogram recursion detected  */
,3,50,255,17
/* two main program units in the same file  */
,13,255,255,31,171,102,255,255,38,3,27,255,20,31
/* only one unnamed %s1 is allowed in an executable program  */
,12,59,125,255,255,40,0,4,19,3,15,233,102
/* function referenced as a subroutine  */
,6,12,255,18,44,9,132
};
static const char __FAR Msg9216[] = {
/* substringing of function or statement function return value is not FORTRAN 77 standard  */
 12,255,24,5,12,13,2,12,130,188,4,1,6
/* substringing valid only for character variables and array elements  */
,12,255,24,255,255,42,59,35,23,88,51,38,227
};
static const char __FAR Msg9472[] = {
/* statement number %i1 has already been defined in line %d2  */
 10,2,26,42,24,57,39,52,3,30,41
/* statement function definition appears after first executable statement  */
,8,2,12,53,69,144,237,233,2
/* %s1 statement must not be branched to but was in line %d2  */
,13,0,2,14,1,7,255,128,8,45,62,3,30,41
/* branch to statement %i1 in line %d2 not allowed  */
,9,58,8,2,42,3,30,41,1,19
/* specification statement must appear before %s1 is initialized  */
,8,49,2,14,79,111,0,4,121
/* statement %i1 was referenced as a FORMAT statement in line %d2  */
,12,2,42,62,255,18,44,9,90,2,3,30,41
/* IMPLICIT statement appears too late  */
,5,107,2,69,104,253
/* this statement will never be executed due to the preceding branch  */
,12,55,2,255,31,7,234,159,8,27,255,14,58
/* expecting statement number  */
,3,10,2,26
/* statement number %i1 was not used as a FORMAT statement in line %d2  */
,13,2,26,42,62,1,87,44,9,90,2,3,30,41
/* specification statement appears too late  */
,5,49,2,69,104,253
/* %s1 statement not allowed after %s2 statement  */
,7,0,2,1,19,144,11,2
/* statement number must be 99999 or less  */
,9,2,26,14,7,255,60,13,255,204
/* statement number cannot be zero  */
,5,2,26,16,7,105
/* this statement could branch to itself  */
,7,55,2,222,58,8,255,199
/* missing statement number %i1 - used in line %d2  */
,9,25,2,26,42,56,87,3,30,41
/* undecodeable statement or misspelled word %s1  */
,8,255,255,34,2,13,255,214,0
/* statement %i1 will never be executed due to the preceding branch  */
,12,2,42,255,31,7,234,159,8,27,255,14,58
/* expecting keyword or symbolic name  */
,6,10,255,201,13,133,18
/* number in %s1 statement is longer than 5 digits  */
,10,26,3,0,2,4,255,3,77,255,57
/* position of DATA statement is not FORTRAN 77 standard  */
,8,255,235,5,142,2,4,1,6
};
static const char __FAR Msg9728[] = {
/* variable %s1 in array declarator must be in COMMON or a dummy argument  */
 13,21,0,3,38,223,14,7,3,22,13,9,73,17
/* adjustable/assumed size array %s1 must be a dummy argument  */
,10,255,107,40,38,0,14,7,9,73,17
/* invalid subscript expression  */
,5,20,255,255,17,64
/* invalid number of subscripts  */
,6,20,26,5,255,255,19
/* using %s1 name incorrectly without list  */
,8,187,0,18,165,255,255,48,48
/* cannot substring array name %s1  */
,5,16,183,38,18,0
/* %s1 treated as an assumed size array  */
,9,0,255,255,30,44,15,109,40,38
/* assumed size array %s1 cannot be used as an i/o list item or a format/unit identifier  */
,17,109,40,38,0,16,7,87,44,15,162,48,167,13,9,255,180,245
/* limit of 65535 elements per dimension has been exceeded  */
,11,84,5,255,59,227,255,233,224,24,39,161
};
static const char __FAR Msg9984[] = {
/* unexpected number or name %s1  */
 5,184,26,13,18,0
/* bad sequence of operators  */
,6,255,124,180,5,255,228
/* invalid operator  */
,2,20,60
/* expecting end of statement after right parenthesis  */
,8,10,74,5,2,144,255,252,126
/* expecting an asterisk  */
,3,10,15,146
/* expecting colon  */
,2,10,216
/* expecting colon or end of statement  */
,6,10,216,13,74,5,2
/* missing comma  */
,2,25,113
/* expecting end of statement  */
,4,10,74,5,2
/* expecting integer variable  */
,3,10,65,21
/* expecting %s1 name  */
,3,10,0,18
/* expecting an integer  */
,3,10,15,65
/* expecting INTEGER, REAL, or DOUBLE PRECISION variable  */
,5,10,202,13,106,21
/* missing operator  */
,2,25,60
/* expecting a slash  */
,5,10,9,255,255,6
/* expecting %s1 expression  */
,3,10,0,64
/* expecting a constant expression  */
,4,10,9,34,64
/* expecting INTEGER, REAL, or DOUBLE PRECISION expression  */
,5,10,202,13,106,64
/* expecting INTEGER or CHARACTER constant  */
,6,10,255,82,13,141,34
/* unexpected operator  */
,2,184,60
/* no closing quote on literal string  */
,8,76,151,255,16,85,255,209,182
/* missing or invalid constant  */
,4,25,13,20,34
/* expecting character constant  */
,3,10,23,34
};
static const char __FAR Msg10240[] = {
/* length specification before array declarator is not FORTRAN 77 standard  */
 8,47,49,111,38,223,4,1,6
/* %i1 is an illegal length for %s2 type  */
,8,42,4,15,36,47,35,11,33
/* length specifier in %s1 statement is not FORTRAN 77 standard  */
,8,47,61,3,0,2,4,1,6
/* length specification not allowed with type %s1  */
,7,47,49,1,19,28,33,0
/* type of %s1 has already been established as %s2  */
,10,33,5,0,24,57,39,255,164,44,11
/* type of %s1 has not been declared  */
,7,33,5,0,24,1,39,154
/* %s1 of type %s2 is illegal in %s3 statement  */
,9,0,5,33,11,4,36,3,137,2
};
static const char __FAR Msg10752[] = {
/* illegal use of %s1 name %s2 in %s3 statement  */
 9,36,134,5,0,18,11,3,137,2
/* symbolic name %s1 is longer than 6 characters  */
,10,133,18,0,4,255,3,77,255,58,149
/* %s1 has already been defined as a %s2  */
,8,0,24,57,39,52,44,9,11
/* %s1 %s2 has not been defined  */
,6,0,11,24,1,39,52
/* %s1 is an unreferenced symbol  */
,7,0,4,15,255,29,255,25
/* %s1 already belongs to this NAMELIST group  */
,8,0,57,255,126,8,55,204,240
/* %s1 has been used but not defined  */
,7,0,24,39,87,45,1,52
/* dynamically allocating %s1 is not FORTRAN 77 standard  */
,6,255,158,0,4,1,6
/* %s1 in NAMELIST %s2 is illegal  */
,6,0,3,204,11,4,36
};
static const char __FAR Msg11008[] = {
/* Usage: %s1 {- or /<option>} <file-spec> {- or /<option>}  */
 13,255,99,0,255,32,13,192,255,61,255,32,13,192
,0
/* generate a listing file  */
,4,75,9,123,31
/* listing file to printer  */
,5,123,31,8,255,239
/* listing file to terminal  */
,6,123,31,8,255,255,25
/* listing file to disk  */
,5,123,31,8,255,155
/* generate an error file  */
,4,75,15,81,31
/* issue extension messages  */
,3,166,236,124
/* issue unreferenced warning  */
,5,166,255,29,255,30
/* issue warning messages  */
,4,166,255,30,124
/* INTEGER/LOGICAL size 2/1  */
,5,255,83,40,255,53
/* list INCLUDE files  */
,3,48,201,97
/* display diagnostic messages  */
,3,255,156,124
/* explicit typing required  */
,2,255,169
/* extend line length  */
,3,235,30,47
,0
/* /trace /bounds  */
,2,255,50
/* define macro  */
,2,255,147
/* relax FORMAT type checking  */
,5,255,19,90,33,150
/* relax wild branch checking  */
,7,255,19,255,255,46,58,150
,0
,0
,0
,0
,0
,0
,0
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
,4,255,255,21,59
/* generate default libraries  */
,4,75,155,255,207
,0
,0
,0
,0
,0
,0
,0
/* line # debugging information  */
,5,30,255,33,95,83
/* full debugging information  */
,4,255,184,95,83
,0
/* no call-ret optimizations  */
,4,76,255,133,66
/* disable optimizations  */
,3,255,154,66
,0
/* statement functions in-line  */
,4,2,255,186,164
/* loop optimizations  */
,2,54,66
/* math optimizations  */
,3,255,211,66
/* numerical optimizations  */
,3,255,224,66
/* precision optimizations  */
,3,255,15,66
/* instruction scheduling  */
,2,255,196
/* space optimizations  */
,4,255,255,7,66
/* time optimizations  */
,3,255,27,66
,0
,0
/* /o[bp,i,l,k,m,r,t,do]  */
,2,255,49
,0
/* set data threshold  */
,5,131,63,255,255,29
/* local variables on the stack  */
,8,255,2,88,85,27,255,255,10
/* pass character descriptors  */
,4,178,23,255,151
/* SAVE local variables  */
,4,206,255,2,88
/* name object file  */
,4,18,255,225,31
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
,5,155,255,255,47,209
,0
/* multithread application  */
,3,255,219,209
,0
/* dynamic link library  */
,4,116,255,208,254
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* structure definition  */
,2,32,53
/* group name  */
,2,240,18
/* statement function  */
,2,2,12
/* statement function definition  */
,3,2,12,53
/* simple variable  */
,4,255,255,5,21
/* variable in COMMON  */
,3,21,3,22
/* equivalenced variable  */
,2,160,21
/* subprogram argument  */
,2,50,17
/* statement function argument  */
,3,2,12,17
/* array  */
,1,38
/* allocated array  */
,3,255,110,38
/* variable or array  */
,3,21,13,38
/* intrinsic function  */
,2,37,12
/* external subprogram  */
,3,255,171,50
,0
/* assignment  */
,1,145
/* arithmetic if  */
,2,255,117
/* at end of statement,  */
,6,70,74,5,255,255,11
/* Defining subprogram: %s1; first ISN is %u2  */
,7,255,71,237,255,85,4,190
/* library member %s1,  */
,4,254,255,212,189
/* column %d1,  */
,3,152,255,35
/* in %s1,  */
,2,3,189
,0
,0
,0
/* %d1 statements, %i2 bytes, %d3 extensions, %d4 warnings, %d5 errors  */
,4,136,255,255,12
/* Code size (in bytes):  */
,6,255,65,40,191,255,132
/* Number of errors:  */
,4,108,5,255,163
/* Compile time (in seconds):  */
,8,255,67,255,27,191,255,255,0
/* Number of warnings:  */
,5,108,5,255,255,43
/* Number of statements compiled:  */
,6,108,5,255,23,255,142
/* Number of extensions:  */
,4,108,5,255,170
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
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
,3,255,42,68
/* *Diagnostic Options*  */
,3,255,41,68
/* *Debugging Options*  */
,3,255,40,68
/* *Miscellaneous Options*  */
,3,255,45,68
/* *Floating-Point Options*  */
,3,255,43,68
/* *Optimizations*  */
,2,255,46
/* *Memory Models*  */
,2,255,44
/* *CPU Targets*  */
,2,255,37
/* *Compile and Link Options*  */
,6,255,39,51,255,89,68
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
/* generate run-time traceback  */
,3,75,255,253
/* generate bounds checking  */
,4,75,255,127,150
/* *Character Set Options*  */
,3,255,38,68
/* Chinese character set  */
,4,255,64,23,131
/* Japanese character set  */
,4,255,86,23,131
/* Korean character set  */
,4,255,87,23,131
/* generate file dependencies  */
,4,75,31,255,149
/* *Application Type*  */
,2,255,36
/* messages in resource file  */
,5,124,3,255,251,31
/* devices are carriage control  */
,6,255,152,80,255,135,72
/* operate quietly  */
,2,255,227
,0
,0
,0
,0
,0
/* generate browse information  */
,4,75,255,129,83
/* extend float-pt. precision  */
,4,235,238,255,15
/* loop optimizations/unrolling  */
,3,54,255,230
/* align COMMON segments  */
,6,255,108,22,255,255,2
/* LF with FF  */
,5,255,88,28,255,78
/* WATCOM debugging information  */
,4,255,101,95,83
/* DWARF debugging information  */
,4,255,70,95,83
/* Codeview debugging info.  */
,5,255,66,95,255,193
,0
,0
/* DO-variables do not overflow  */
,4,255,69,1,177
/* path for INCLUDE files  */
,5,255,232,35,201,97
,0
/* branch prediction  */
,3,58,255,237
/* mangle COMMON segment names  */
,8,255,210,22,255,255,1,255,7
/* promote intrinsic arguments  */
,4,255,240,37,92
/* allow comma separator  */
,6,255,111,113,255,255,4
,0
,0
/* optimize at expense of compile-time  */
,8,255,10,70,255,168,5,255,141
/* move register saves into flow graph  */
,5,255,218,128,255,254
/* loop optimizations/assume loop invariant float-pt. variables are initialized  */
,10,54,255,229,54,255,198,238,88,80,121
};


extern const char __FAR * const __FAR GroupTable[] = {
     Msg0
    ,Msg256
    ,Msg512
    ,Msg768
    ,Msg1024
    ,Msg1280
    ,Msg1536
    ,Msg1792
    ,Msg2048
    ,Msg2304
    ,Msg2560
    ,Msg2816
    ,Msg3072
    ,Msg3328
    ,Msg3584
    ,Msg3840
    ,Msg4096
    ,Msg4352
    ,Msg4608
    ,Msg4864
    ,Msg5120
    ,Msg5376
    ,Msg5632
    ,NULL
    ,Msg6144
    ,Msg6400
    ,Msg6656
    ,Msg6912
    ,Msg7168
    ,Msg7424
    ,Msg7680
    ,Msg7936
    ,Msg8192
    ,Msg8448
    ,Msg8704
    ,Msg8960
    ,Msg9216
    ,Msg9472
    ,Msg9728
    ,Msg9984
    ,Msg10240
    ,NULL
    ,Msg10752
    ,Msg11008
    };


extern const char __FAR ErrWord[] = {
          3,'%','s','1'
/* count=114 Group:offset = AR: 0, AR: 1, AR:12, BD: 0, BD: 1,... */
          ,3,'n','o','t'
/* count= 82 Group:offset = AR:12, BD: 0, BD: 1, CC: 3, CC: 5,... */
          ,9,'s','t','a','t','e','m','e','n','t'
/* count= 81 Group:offset = BD: 1, CC: 1, CC: 2, DA: 6, DO: 0,... */
          ,2,'i','n'
/* count= 69 Group:offset = BD: 0, BD: 1, CC: 0, CC: 1, CC: 2,... */
          ,2,'i','s'
/* count= 59 Group:offset = BD: 0, BD: 1, CC: 3, CC: 5, CC: 6,... */
          ,2,'o','f'
/* count= 52 Group:offset = AR: 0, AR:12, CM: 1, CP:11, DA: 7,... */
          ,19,'F','O','R','T','R','A','N',' ','7'
              ,'7',' ','s','t','a','n','d','a','r','d'
/* count= 37 Group:offset = CC: 3, CC: 5, CC: 6, CC: 7, CM: 1,... */
          ,2,'b','e'
/* count= 33 Group:offset = CC: 4, CV: 1, DA: 0, DA: 1, DA: 3,... */
          ,2,'t','o'
/* count= 33 Group:offset = AR: 0, CC: 2, CP: 3, EC: 0, EC: 1,... */
          ,1,'a'
/* count= 32 Group:offset = BD: 0, BD: 1, CC: 4, CM: 3, CM: 5,... */
          ,9,'e','x','p','e','c','t','i','n','g'
/* count= 31 Group:offset = CO: 2, DO: 5, EQ: 4, FM: 9, GO: 2,... */
          ,3,'%','s','2'
/* count= 29 Group:offset = AR:12, DA: 5, DA:11, EC: 0, EC: 1,... */
          ,8,'f','u','n','c','t','i','o','n'
/* count= 28 Group:offset = AR: 0, EY: 0, EY: 2, LI: 5, LI: 6,... */
          ,2,'o','r'
/* count= 28 Group:offset = DO: 5, FM: 1, FM: 4, FM: 7, IL: 0,... */
          ,4,'m','u','s','t'
/* count= 26 Group:offset = AR:12, CC: 4, CV: 1, DA: 0, DA: 1,... */
          ,2,'a','n'
/* count= 24 Group:offset = CO: 2, DA: 0, DA: 2, IL: 1, IL: 8,... */
          ,6,'c','a','n','n','o','t'
/* count= 23 Group:offset = CO: 1, DA: 3, DA: 5, DA:11, DO: 3,... */
          ,8,'a','r','g','u','m','e','n','t'
/* count= 22 Group:offset = AR: 1, AR:12, CP: 1, CV: 1, LI: 4,... */
          ,4,'n','a','m','e'
/* count= 21 Group:offset = PR: 0, SA: 2, SF: 1, SF: 6, SP: 2,... */
          ,7,'a','l','l','o','w','e','d'
/* count= 20 Group:offset = CN: 2, CO: 5, CV: 0, DO: 1, EX: 0,... */
          ,7,'i','n','v','a','l','i','d'
/* count= 19 Group:offset = AR: 0, CC: 0, CC: 1, CN: 1, CN: 3,... */
          ,8,'v','a','r','i','a','b','l','e'
/* count= 18 Group:offset = CP:12, CV: 0, CV: 1, DA: 0, DA: 2,... */
          ,6,'C','O','M','M','O','N'
/* count= 16 Group:offset = BD: 0, CM: 0, CM: 1, CM: 2, CM: 3,... */
          ,9,'c','h','a','r','a','c','t','e','r'
/* count= 16 Group:offset = CC: 0, CC: 1, CC: 2, CC: 3, CM: 2,... */
          ,3,'h','a','s'
/* count= 16 Group:offset = CM: 3, CM: 5, CP: 2, EC: 0, EV: 0,... */
          ,7,'m','i','s','s','i','n','g'
/* count= 16 Group:offset = EN: 0, FM: 0, FM: 1, FM: 3, FM: 4,... */
          ,6,'n','u','m','b','e','r'
/* count= 16 Group:offset = AR: 0, CC: 1, CO: 3, DO: 0, FM:11,... */
          ,3,'t','h','e'
/* count= 16 Group:offset = CO: 2, CO: 3, EC: 0, IF: 0, IL: 1,... */
          ,4,'w','i','t','h'
/* count= 16 Group:offset = CM: 3, CM: 5, CP: 1, CV: 0, CV: 1,... */
          ,5,'b','l','o','c','k'
/* count= 15 Group:offset = BD: 0, CM: 1, CM: 3, CM: 4, CM: 5,... */
          ,4,'l','i','n','e'
/* count= 15 Group:offset = CC: 4, DO: 0, GO: 0, GO: 1, PR: 3,... */
          ,4,'f','i','l','e'
/* count= 14 Group:offset = IL: 1, IL:10, SM: 0, SM: 1, SM: 2,... */
          ,9,'s','t','r','u','c','t','u','r','e'
/* count= 14 Group:offset = SF: 6, SP: 3, SP: 4, SP: 7, SP: 8,... */
          ,4,'t','y','p','e'
/* count= 14 Group:offset = EV: 5, EY: 0, IM: 2, LI:11, LI:14,... */
          ,8,'c','o','n','s','t','a','n','t'
/* count= 13 Group:offset = CN: 1, CN: 3, CN: 4, DA: 4, DA:11,... */
          ,3,'f','o','r'
/* count= 13 Group:offset = CO: 6, DA: 7, DA: 9, FM:10, HO: 1,... */
          ,7,'i','l','l','e','g','a','l'
/* count= 13 Group:offset = CP: 5, EQ: 0, EQ: 2, IM: 0, PR: 1,... */
          ,9,'i','n','t','r','i','n','s','i','c'
/* count= 13 Group:offset = AR: 0, LI: 5, LI: 6, LI: 9, LI:10,... */
          ,5,'a','r','r','a','y'
/* count= 12 Group:offset = IL:17, LI:16, SS: 1, SV: 0, SV: 1,... */
          ,4,'b','e','e','n'
/* count= 12 Group:offset = CM: 3, CM: 5, EV: 0, SP:23, SP:24,... */
          ,4,'s','i','z','e'
/* count= 12 Group:offset = CM: 3, CM: 5, EY: 2, IL: 3, PR: 1,... */
          ,3,'%','d','2'
/* count= 11 Group:offset = DO: 0, GO: 0, GO: 1, SP: 8, SP: 9,... */
          ,3,'%','i','1'
/* count= 11 Group:offset = DO: 0, GO: 1, SP: 8, SP: 9, ST: 0,... */
          ,2,'D','O'
/* count= 11 Group:offset = DA: 0, DA: 2, DO: 0, DO: 1, DO: 2,... */
          ,2,'a','s'
/* count= 11 Group:offset = IL:17, LI: 9, LI:12, SR: 1, SR:11,... */
          ,3,'b','u','t'
/* count= 11 Group:offset = BD: 0, CC: 2, DA: 2, EV: 5, FM: 9,... */
          ,6,'f','o','r','m','a','t'
/* count= 11 Group:offset = FM: 0, FM: 2, FM: 4, FM: 5, FM: 7,... */
          ,6,'l','e','n','g','t','h'
/* count= 11 Group:offset = CN: 2, CV: 0, CV: 1, IM: 3, MO: 2,... */
          ,4,'l','i','s','t'
/* count= 11 Group:offset = DA: 7, DA: 9, EV: 1, IL: 4, IL: 5,... */
          ,13,'s','p','e','c','i','f','i','c','a'
              ,'t','i','o','n'
/* count= 11 Group:offset = DA: 1, FM: 6, FM:10, FM:14, IL: 0,... */
          ,10,'s','u','b','p','r','o','g','r','a'
              ,'m'
/* count= 11 Group:offset = BD: 0, BD: 1, CM: 1, CM: 4, CP: 2,... */
          ,3,'a','n','d'
/* count= 10 Group:offset = CM: 2, EC: 1, EV: 4, IL:12, MD: 1,... */
          ,7,'d','e','f','i','n','e','d'
/* count= 10 Group:offset = CM: 3, CM: 5, DO: 0, SA: 0, SP:23,... */
          ,10,'d','e','f','i','n','i','t','i','o'
              ,'n'
/* count= 10 Group:offset = AR:12, SF: 0, SF: 2, SP:20, SP:22,... */
          ,4,'l','o','o','p'
/* count= 10 Group:offset = DO: 0, DO: 2, DO: 3, DO: 4, DO: 6,... */
          ,4,'t','h','i','s'
/* count= 10 Group:offset = CV: 0, DO: 4, FM:10, IL: 1, LI:11,... */
          ,1,'-'
/* count=  9 Group:offset = CO: 4, CP: 3, DO: 0, MO: 2, SM: 0,... */
          ,7,'a','l','r','e','a','d','y'
/* count=  9 Group:offset = CM: 0, CO: 0, DO: 0, SA: 2, SP:24,... */
          ,6,'b','r','a','n','c','h'
/* count=  9 Group:offset = SP: 8, SP: 9, SP:12, ST: 3, ST: 7,... */
          ,4,'o','n','l','y'
/* count=  9 Group:offset = IM: 1, MD: 4, RE: 0, SR: 0, SR: 2,... */
          ,8,'o','p','e','r','a','t','o','r'
/* count=  9 Group:offset = MD: 0, MD: 2, MD: 3, MD: 5, MD: 6,... */
          ,9,'s','p','e','c','i','f','i','e','r'
/* count=  9 Group:offset = IL:12, IL:14, IL:15, IL:17, IM: 3,... */
          ,3,'w','a','s'
/* count=  9 Group:offset = BD: 0, LI: 7, LI:13, LI:14, SF: 4,... */
          ,4,'d','a','t','a'
/* count=  8 Group:offset = BD: 0, CM: 1, CM: 2, DA: 4, DA: 6,... */
          ,10,'e','x','p','r','e','s','s','i','o'
              ,'n'
/* count=  8 Group:offset = CV: 0, DA: 2, DM: 0, SP:29, SV: 2,... */
          ,7,'i','n','t','e','g','e','r'
/* count=  8 Group:offset = DA: 0, DA: 1, KO: 3, KO: 5, MD: 6,... */
          ,13,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s'
/* count=  8 Group:offset = MS:48, MS:49, MS:52, MS:53, MS:54,... */
          ,5,'B','L','O','C','K'
/* count=  7 Group:offset = BD: 1, CM: 4, SP: 4, SP:12, SP:13,... */
          ,8,'O','p','t','i','o','n','s','*'
/* count=  7 Group:offset = MS:161, MS:162, MS:163, MS:164, MS:165,... */
          ,7,'a','p','p','e','a','r','s'
/* count=  7 Group:offset = AR: 1, CM: 4, DA: 2, SA: 1, ST: 1,... */
          ,2,'a','t'
/* count=  7 Group:offset = DO: 1, EV: 1, SP: 7, SP:19, SP:25,... */
          ,4,'c','o','d','e'
/* count=  7 Group:offset = FM: 2, FM: 7, FM:10, FM:15, FM:20,... */
          ,7,'c','o','n','t','r','o','l'
/* count=  7 Group:offset = EY: 1, IL: 4, IL: 5, SP: 4, SP: 8,... */
          ,5,'d','u','m','m','y'
/* count=  7 Group:offset = AR: 1, AR:12, SF: 0, SF: 2, SF: 4,... */
          ,3,'e','n','d'
/* count=  7 Group:offset = FM: 9, SP:10, SX: 3, SX: 6, SX: 8,... */
          ,8,'g','e','n','e','r','a','t','e'
/* count=  7 Group:offset = MS: 2, MS: 6, MS:37, MS:192, MS:193,... */
          ,2,'n','o'
/* count=  7 Group:offset = CC: 2, FM:11, FM:12, IL: 6, SP:21,... */
          ,4,'t','h','a','n'
/* count=  7 Group:offset = AR: 1, CC: 5, CM: 4, FM:13, IL: 5,... */
          ,5,'\'','%','s','1','\''
/* count=  6 Group:offset = CO: 1, CO: 3, CO: 4, CO: 5, PR: 4,... */
          ,6,'a','p','p','e','a','r'
/* count=  6 Group:offset = AR:12, GO: 0, SR: 0, SR: 3, SR: 7,... */
          ,3,'a','r','e'
/* count=  6 Group:offset = CN: 0, CN: 2, EC: 1, SP: 6, MS:201,... */
          ,5,'e','r','r','o','r'
/* count=  6 Group:offset = CP: 4, SM: 0, SM: 1, SM: 2, SM: 5,... */
          ,5,'f','i','e','l','d'
/* count=  6 Group:offset = FM: 8, FM:13, FM:14, SP:21, SP:22,... */
          ,11,'i','n','f','o','r','m','a','t','i'
              ,'o','n'
/* count=  6 Group:offset = SM: 6, MS:45, MS:46, MS:208, MS:213,... */
          ,5,'l','i','m','i','t'
/* count=  6 Group:offset = CP:10, CP:11, CP:12, MO: 0, PR: 5,... */
          ,2,'o','n'
/* count=  6 Group:offset = CP: 3, FM:11, FM:12, SP: 9, SX:20,... */
          ,4,'o','n','c','e'
/* count=  6 Group:offset = AR: 1, IL: 5, IM: 1, IM: 4, SR: 0,... */
          ,4,'u','s','e','d'
/* count=  6 Group:offset = SP:30, SR: 2, ST: 9, ST:15, SV: 7,... */
          ,9,'v','a','r','i','a','b','l','e','s'
/* count=  6 Group:offset = DA: 7, DA: 9, SS: 1, MS:64, MS:66,... */
          ,3,'(','*',')'
/* count=  5 Group:offset = CV: 0, CV: 1, IM: 3, SF: 1, SR: 1 */
          ,6,'F','O','R','M','A','T'
/* count=  5 Group:offset = FM: 9, FM:11, ST: 5, ST: 9, MS:18 */
          ,6,'R','E','M','O','T','E'
/* count=  5 Group:offset = SP: 4, SP:12, SP:13, SP:14, SP:15 */
          ,9,'a','r','g','u','m','e','n','t','s'
/* count=  5 Group:offset = AR: 0, LI: 5, SF: 0, SF: 5, MS:223 */
          ,3,'c','a','n'
/* count=  5 Group:offset = IM: 1, MD: 4, SR: 0, SR: 2, SR: 7 */
          ,12,'c','o','n','t','i','n','u','a','t'
              ,'i','o','n'
/* count=  5 Group:offset = CC: 2, CC: 4, CC: 5, CC: 8, PR: 3 */
          ,9,'d','e','b','u','g','g','i','n','g'
/* count=  5 Group:offset = MS:45, MS:46, MS:213, MS:214, MS:215 */
          ,7,'e','x','c','e','e','d','s'
/* count=  5 Group:offset = CP:10, CP:11, CP:12, MO: 0, SP:10 */
          ,5,'f','i','l','e','s'
/* count=  5 Group:offset = IL: 7, IL:13, SM: 5, MS:11, MS:219 */
          ,5,'f','o','u','n','d'
/* count=  5 Group:offset = FM: 9, LI: 7, LI:13, SP:19, SP:21 */
          ,6,'m','e','m','o','r','y'
/* count=  5 Group:offset = CP: 3, CP: 8, CP: 9, MO: 1, MO: 2 */
          ,4,'m','o','r','e'
/* count=  5 Group:offset = AR: 1, CC: 5, CM: 4, FM: 9, IL: 5 */
          ,6,'o','p','t','i','o','n'
/* count=  5 Group:offset = CO: 1, CO: 2, CO: 3, CO: 4, CO: 5 */
          ,7,'p','r','o','g','r','a','m'
/* count=  5 Group:offset = KO: 4, RE: 1, SR: 5, SR: 9, SR:10 */
          ,5,'r','a','n','g','e'
/* count=  5 Group:offset = DO: 1, IM: 0, LI: 4, SP: 4, SP:10 */
          ,3,'t','o','o'
/* count=  5 Group:offset = CC: 8, DA: 9, SM: 5, ST: 6, ST:10 */
          ,4,'z','e','r','o'
/* count=  5 Group:offset = CN: 2, KO: 0, KO: 3, LI: 4, ST:13 */
          ,16,'D','O','U','B','L','E',' ','P','R'
              ,'E','C','I','S','I','O','N'
/* count=  4 Group:offset = CN: 0, MD: 1, SX:12, SX:17 */
          ,8,'I','M','P','L','I','C','I','T'
/* count=  4 Group:offset = IM: 3, IM: 4, ST: 6 */
          ,6,'N','u','m','b','e','r'
/* count=  4 Group:offset = MS:123, MS:125, MS:126, MS:127 */
          ,7,'a','s','s','u','m','e','d'
/* count=  4 Group:offset = FM: 0, FM:19, SV: 6, SV: 7 */
          ,7,'a','t','t','e','m','p','t'
/* count=  4 Group:offset = EV: 5, IL: 5, SP:12, SP:13 */
          ,6,'b','e','f','o','r','e'
/* count=  4 Group:offset = AR:12, FM:19, ST: 4, TY: 0 */
          ,2,'b','y'
/* count=  4 Group:offset = CP: 5, KO: 0, KO: 3, SP: 5 */
          ,5,'c','o','m','m','a'
/* count=  4 Group:offset = DO: 5, FM: 0, SX: 7, MS:224 */
          ,8,'c','o','m','p','i','l','e','r'
/* count=  4 Group:offset = CO: 6, CO: 7, CP: 4, MO: 0 */
          ,9,'c','o','n','s','t','a','n','t','s'
/* count=  4 Group:offset = CN: 0, CN: 2, DA: 7, DA: 9 */
          ,7,'d','y','n','a','m','i','c'
/* count=  4 Group:offset = CP: 8, CP: 9, MO: 2, MS:81 */
          ,4,'e','d','i','t'
/* count=  4 Group:offset = FM: 4, FM: 5, FM:12, FM:19 */
          ,14,'f','l','o','a','t','i','n','g','-'
              ,'p','o','i','n','t'
/* count=  4 Group:offset = CN: 1, KO: 0, KO: 1, KO: 2 */
          ,4,'f','r','o','m'
/* count=  4 Group:offset = KO: 4, SP: 3, SP: 8, SP:15 */
          ,4,'h','a','v','e'
/* count=  4 Group:offset = IL: 2, IL: 3, IL:12, SP: 0 */
          ,11,'i','n','i','t','i','a','l','i','z'
              ,'e','d'
/* count=  4 Group:offset = BD: 0, DA: 3, ST: 4, MS:229 */
          ,8,'i','n','t','e','r','n','a','l'
/* count=  4 Group:offset = CP: 4, IL: 1, IL: 7, IL:13 */
          ,7,'l','i','s','t','i','n','g'
/* count=  4 Group:offset = MS: 2, MS: 3, MS: 4, MS: 5 */
          ,8,'m','e','s','s','a','g','e','s'
/* count=  4 Group:offset = MS: 7, MS: 9, MS:12, MS:200 */
          ,3,'o','n','e'
/* count=  4 Group:offset = CM: 4, SP: 7, SP:25, SR:10 */
          ,11,'p','a','r','e','n','t','h','e','s'
              ,'i','s'
/* count=  4 Group:offset = PC: 0, PC: 1, PC: 2, SX: 3 */
          ,6,'p','a','s','s','e','d'
/* count=  4 Group:offset = LI: 5, LI:12, SF: 4, SF: 5 */
          ,8,'r','e','g','i','s','t','e','r'
/* count=  4 Group:offset = CP: 1, CP: 2, CP: 5, MS:228 */
          ,8,'r','e','q','u','i','r','e','s'
/* count=  4 Group:offset = CO: 3, IL: 8, LI:10, SP: 7 */
          ,6,'r','e','t','u','r','n'
/* count=  4 Group:offset = CP: 2, CP:11, RE: 0, SS: 0 */
          ,3,'s','e','t'
/* count=  4 Group:offset = MS:63, MS:195, MS:196, MS:197 */
          ,10,'s','u','b','r','o','u','t','i','n'
              ,'e'
/* count=  4 Group:offset = RE: 0, SR: 2, SR: 4, SR:11 */
          ,8,'s','y','m','b','o','l','i','c'
/* count=  4 Group:offset = PR: 0, SP: 2, ST:18, VA: 1 */
          ,3,'u','s','e'
/* count=  4 Group:offset = EQ: 2, SP:14, SP:27, VA: 0 */
          ,6,'w','i','t','h','i','n'
/* count=  4 Group:offset = EY: 1, SP:15, SP:28, SP:30 */
          ,3,'%','d','1'
/* count=  3 Group:offset = CP: 1, CP: 4, MS:121 */
          ,3,'%','s','3'
/* count=  3 Group:offset = DA:11, TY: 6, VA: 0 */
          ,1,'1'
/* count=  3 Group:offset = CC: 7, FM:19, PR: 3 */
          ,1,'2'
/* count=  3 Group:offset = EV: 0, EV: 1, LI: 5 */
          ,3,'3','2','K'
/* count=  3 Group:offset = CP:10, CP:11, CP:12 */
          ,9,'C','H','A','R','A','C','T','E','R'
/* count=  3 Group:offset = CV: 0, EV: 4, SX:18 */
          ,4,'D','A','T','A'
/* count=  3 Group:offset = BD: 1, CM: 4, ST:20 */
          ,3,'T','h','e'
/* count=  3 Group:offset = CP:10, CP:11, CP:12 */
          ,5,'a','f','t','e','r'
/* count=  3 Group:offset = ST: 1, ST:11, SX: 3 */
          ,10,'a','s','s','i','g','n','m','e','n'
              ,'t'
/* count=  3 Group:offset = EQ: 0, EQ: 3, MS:111 */
          ,8,'a','s','t','e','r','i','s','k'
/* count=  3 Group:offset = IL: 8, IL:11, SX: 4 */
          ,5,'b','l','a','n','k'
/* count=  3 Group:offset = CC: 4, CM: 5, DA: 3 */
          ,4,'b','o','t','h'
/* count=  3 Group:offset = IL:12, SP: 0, SR: 3 */
          ,10,'c','h','a','r','a','c','t','e','r'
              ,'s'
/* count=  3 Group:offset = HO: 1, IM: 0, VA: 1 */
          ,8,'c','h','e','c','k','i','n','g'
/* count=  3 Group:offset = MS:18, MS:19, MS:193 */
          ,7,'c','l','o','s','i','n','g'
/* count=  3 Group:offset = FM:12, PC: 0, SX:20 */
          ,6,'c','o','l','u','m','n'
/* count=  3 Group:offset = CC: 7, PR: 3, MS:116 */
          ,7,'c','o','n','t','a','i','n'
/* count=  3 Group:offset = EV: 1, PR: 3, SP:25 */
          ,8,'d','e','c','l','a','r','e','d'
/* count=  3 Group:offset = IM: 1, SR: 1, TY: 5 */
          ,7,'d','e','f','a','u','l','t'
/* count=  3 Group:offset = SP:11, MS:37, MS:77 */
          ,10,'d','e','s','c','r','i','p','t','o'
              ,'r'
/* count=  3 Group:offset = FM: 4, FM: 5, FM:12 */
          ,9,'d','i','f','f','e','r','e','n','t'
/* count=  3 Group:offset = CM: 3, CM: 5, EV: 0 */
          ,4,'d','o','e','s'
/* count=  3 Group:offset = EY: 0, EY: 2, SP:18 */
          ,3,'d','u','e'
/* count=  3 Group:offset = MO: 2, ST: 7, ST:17 */
          ,12,'e','q','u','i','v','a','l','e','n'
              ,'c','e','d'
/* count=  3 Group:offset = EC: 1, EV: 0, MS:102 */
          ,8,'e','x','c','e','e','d','e','d'
/* count=  3 Group:offset = CO: 6, PR: 5, SV: 8 */
          ,3,'i','/','o'
/* count=  3 Group:offset = IL:15, IL:16, SV: 7 */
          ,7,'i','m','p','l','i','e','d'
/* count=  3 Group:offset = DA: 0, DA: 2, IL: 6 */
          ,7,'i','n','-','l','i','n','e'
/* count=  3 Group:offset = PR: 5, PR: 7, MS:51 */
          ,11,'i','n','c','o','r','r','e','c','t'
              ,'l','y'
/* count=  3 Group:offset = DM: 0, EV: 2, SV: 4 */
          ,5,'i','s','s','u','e'
/* count=  3 Group:offset = MS: 7, MS: 8, MS: 9 */
          ,4,'i','t','e','m'
/* count=  3 Group:offset = IL: 4, IL: 5, SV: 7 */
          ,5,'l','a','b','e','l'
/* count=  3 Group:offset = DO: 3, GO: 0, SP: 2 */
          ,5,'l','e','a','s','t'
/* count=  3 Group:offset = EV: 1, SP: 7, SP:25 */
          ,7,'l','o','g','i','c','a','l'
/* count=  3 Group:offset = MD: 0, MD: 6, MD: 7 */
          ,4,'m','a','i','n'
/* count=  3 Group:offset = RE: 1, SR: 5, SR: 9 */
          ,4,'m','a','n','y'
/* count=  3 Group:offset = CC: 8, DA: 9, SM: 5 */
          ,5,'m','a','t','c','h'
/* count=  3 Group:offset = EY: 0, EY: 2, SP:18 */
          ,3,'m','a','y'
/* count=  3 Group:offset = GO: 0, IL: 1, SR: 1 */
          ,7,'o','p','e','n','i','n','g'
/* count=  3 Group:offset = PC: 1, SM: 1, SM: 5 */
          ,8,'o','p','e','r','a','n','d','s'
/* count=  3 Group:offset = MD: 2, MD: 3, MD: 6 */
          ,8,'o','v','e','r','f','l','o','w'
/* count=  3 Group:offset = KO: 1, KO: 5, MS:218 */
          ,4,'p','a','s','s'
/* count=  3 Group:offset = LI: 9, SF: 3, MS:65 */
          ,6,'r','e','p','e','a','t'
/* count=  3 Group:offset = DA: 1, FM: 6, FM:10 */
          ,8,'s','e','q','u','e','n','c','e'
/* count=  3 Group:offset = PR: 5, PR: 6, SX: 1 */
          ,4,'s','i','g','n'
/* count=  3 Group:offset = CO: 2, EQ: 2, EQ: 4 */
          ,6,'s','t','r','i','n','g'
/* count=  3 Group:offset = FM: 4, FM: 5, SX:20 */
          ,9,'s','u','b','s','t','r','i','n','g'
/* count=  3 Group:offset = EV: 3, EV: 5, SV: 5 */
          ,10,'u','n','e','x','p','e','c','t','e'
              ,'d'
/* count=  3 Group:offset = PC: 2, SX: 0, SX:19 */
          ,4,'u','n','i','t'
/* count=  3 Group:offset = IL: 9, IL:10, IL:11 */
          ,14,'u','n','r','e','c','o','g','n','i'
              ,'z','a','b','l','e'
/* count=  3 Group:offset = FM: 5, IL: 4, IM: 2 */
          ,5,'u','s','i','n','g'
/* count=  3 Group:offset = DM: 0, MD: 4, SV: 4 */
          ,5,'v','a','l','u','e'
/* count=  3 Group:offset = CP:11, EQ: 1, SS: 0 */
          ,4,'%','s','1',','
/* count=  2 Group:offset = MS:115, MS:117 */
          ,3,'%','u','2'
/* count=  2 Group:offset = MO: 0, MS:114 */
          ,3,'(','i','n'
/* count=  2 Group:offset = MS:122, MS:124 */
          ,10,'/','<','o','p','t','i','o','n','>'
              ,'}'
/* count=  2 Group:offset = MS: 0 */
          ,6,'A','S','S','I','G','N'
/* count=  2 Group:offset = GO: 0, GO: 1 */
          ,5,'A','T','E','N','D'
/* count=  2 Group:offset = SP: 0, SP: 1 */
          ,4,'C','A','S','E'
/* count=  2 Group:offset = SP: 5, SP: 7 */
          ,7,'C','O','M','P','L','E','X'
/* count=  2 Group:offset = CN: 0, MD: 1 */
          ,3,'E','N','D'
/* count=  2 Group:offset = EN: 0, IL:14 */
          ,5,'E','N','T','R','Y'
/* count=  2 Group:offset = AR:12, EY: 1 */
          ,11,'E','Q','U','I','V','A','L','E','N'
              ,'C','E'
/* count=  2 Group:offset = EV: 1, EV: 5 */
          ,8,'E','X','T','E','R','N','A','L'
/* count=  2 Group:offset = SR: 0, SR: 3 */
          ,7,'I','N','C','L','U','D','E'
/* count=  2 Group:offset = MS:11, MS:219 */
          ,14,'I','N','T','E','G','E','R',',',' '
              ,'R','E','A','L',','
/* count=  2 Group:offset = SX:12, SX:17 */
          ,9,'I','N','T','R','I','N','S','I','C'
/* count=  2 Group:offset = SR: 3, SR: 7 */
          ,8,'N','A','M','E','L','I','S','T'
/* count=  2 Group:offset = VA: 5, VA: 8 */
          ,6,'R','E','T','U','R','N'
/* count=  2 Group:offset = RE: 1, SP:15 */
          ,4,'S','A','V','E'
/* count=  2 Group:offset = SA: 2, MS:66 */
          ,4,'U','N','I','T'
/* count=  2 Group:offset = IL: 1, IL:12 */
          ,11,'a','l','l','o','c','a','t','a','b'
              ,'l','e'
/* count=  2 Group:offset = LI:16, SP:28 */
          ,11,'a','p','p','l','i','c','a','t','i'
              ,'o','n'
/* count=  2 Group:offset = MS:77, MS:79 */
          ,9,'a','r','g','u','m','e','n','t',','
/* count=  2 Group:offset = LI: 7, LI:13 */
          ,9,'a','t','t','r','i','b','u','t','e'
/* count=  2 Group:offset = PR: 1, PR: 2 */
          ,9,'a','u','t','o','m','a','t','i','c'
/* count=  2 Group:offset = CP:10, CP:12 */
          ,6,'b','l','o','c','k','s'
/* count=  2 Group:offset = EY: 1, SP:11 */
          ,4,'b','y','t','e'
/* count=  2 Group:offset = PR: 5, PR: 6 */
          ,4,'c','a','s','e'
/* count=  2 Group:offset = SP:11 */
          ,5,'c','o','l','o','n'
/* count=  2 Group:offset = SX: 5, SX: 6 */
          ,7,'c','o','l','u','m','n','s'
/* count=  2 Group:offset = CC: 1, CC: 4 */
          ,7,'c','o','m','m','e','n','t'
/* count=  2 Group:offset = CC: 6, PR: 3 */
          ,7,'c','o','m','p','l','e','x'
/* count=  2 Group:offset = LI: 5, MD: 4 */
          ,10,'c','o','n','t','a','i','n','i','n'
              ,'g'
/* count=  2 Group:offset = CP:10, SP:27 */
          ,8,'c','o','n','t','a','i','n','s'
/* count=  2 Group:offset = SF: 0, SF: 2 */
          ,5,'c','o','u','l','d'
/* count=  2 Group:offset = SP: 3, ST:14 */
          ,10,'d','e','c','l','a','r','a','t','o'
              ,'r'
/* count=  2 Group:offset = SV: 0, TY: 0 */
          ,9,'d','i','m','e','n','s','i','o','n'
/* count=  2 Group:offset = DM: 0, SV: 8 */
          ,10,'d','i','r','e','c','t','i','v','e'
              ,'s'
/* count=  2 Group:offset = CO: 6, CO: 7 */
          ,6,'d','i','v','i','d','e'
/* count=  2 Group:offset = KO: 0, KO: 3 */
          ,8,'e','l','e','m','e','n','t','s'
/* count=  2 Group:offset = SS: 1, SV: 8 */
          ,11,'e','n','c','o','u','n','t','e','r'
              ,'e','d'
/* count=  2 Group:offset = CC: 0, CC: 3 */
          ,6,'e','n','o','u','g','h'
/* count=  2 Group:offset = DA: 7, HO: 1 */
          ,5,'e','n','t','r','y'
/* count=  2 Group:offset = EY: 0, EY: 2 */
          ,6,'e','q','u','a','l','s'
/* count=  2 Group:offset = CO: 2, EQ: 4 */
          ,13,'e','q','u','i','v','a','l','e','n'
              ,'c','i','n','g'
/* count=  2 Group:offset = EC: 0, EV: 4 */
          ,10,'e','x','e','c','u','t','a','b','l'
              ,'e'
/* count=  2 Group:offset = SR:10, ST: 1 */
          ,8,'e','x','e','c','u','t','e','d'
/* count=  2 Group:offset = ST: 7, ST:17 */
          ,6,'e','x','t','e','n','d'
/* count=  2 Group:offset = MS:14, MS:209 */
          ,9,'e','x','t','e','n','s','i','o','n'
/* count=  2 Group:offset = EC: 0, MS: 7 */
          ,5,'f','i','r','s','t'
/* count=  2 Group:offset = ST: 1, MS:114 */
          ,9,'f','l','o','a','t','-','p','t','.'
/* count=  2 Group:offset = MS:209, MS:229 */
          ,6,'f','o','l','l','o','w'
/* count=  2 Group:offset = SP: 1, SP:11 */
          ,5,'g','r','o','u','p'
/* count=  2 Group:offset = VA: 5, MS:97 */
          ,11,'h','e','x','a','d','e','c','i','m'
              ,'a','l'
/* count=  2 Group:offset = DA: 4, PR: 6 */
          ,17,'h','e','x','a','d','e','c','i','m'
              ,'a','l','/','o','c','t','a','l'
/* count=  2 Group:offset = CN: 3, CN: 4 */
          ,9,'h','o','l','l','e','r','i','t','h'
/* count=  2 Group:offset = HO: 0, HO: 1 */
          ,14,'i','d','e','n','t','i','f','i','c'
              ,'a','t','i','o','n'
/* count=  2 Group:offset = IL: 9, IL:10 */
          ,10,'i','d','e','n','t','i','f','i','e'
              ,'r'
/* count=  2 Group:offset = IL:11, SV: 7 */
          ,7,'i','g','n','o','r','e','d'
/* count=  2 Group:offset = CO: 4, MO: 2 */
          ,11,'i','m','m','e','d','i','a','t','e'
              ,'l','y'
/* count=  2 Group:offset = SP: 1, SP: 5 */
          ,8,'i','m','p','r','o','p','e','r'
/* count=  2 Group:offset = DO: 2, IL: 9 */
          ,9,'i','n','c','o','r','r','e','c','t'
/* count=  2 Group:offset = EV: 3, SF: 5 */
          ,14,'i','n','i','t','i','a','l','i','z'
              ,'a','t','i','o','n'
/* count=  2 Group:offset = DA: 4, DA: 6 */
          ,10,'i','n','i','t','i','a','l','i','z'
              ,'e'
/* count=  2 Group:offset = DA: 5, DA:11 */
          ,12,'i','n','p','u','t','/','o','u','t'
              ,'p','u','t'
/* count=  2 Group:offset = IL: 6, IL: 7 */
          ,4,'l','a','t','e'
/* count=  2 Group:offset = ST: 6, ST:10 */
          ,7,'l','i','b','r','a','r','y'
/* count=  2 Group:offset = MS:81, MS:115 */
          ,5,'l','i','n','e','s'
/* count=  2 Group:offset = CC: 5, CC: 8 */
          ,13,'l','i','s','t','-','d','i','r','e'
              ,'c','t','e','d'
/* count=  2 Group:offset = IL: 7, IL: 8 */
          ,5,'l','o','c','a','l'
/* count=  2 Group:offset = MS:64, MS:66 */
          ,6,'l','o','n','g','e','r'
/* count=  2 Group:offset = ST:19, VA: 1 */
          ,3,'l','o','w'
/* count=  2 Group:offset = CP: 3, SP:10 */
          ,9,'m','i','s','p','l','a','c','e','d'
/* count=  2 Group:offset = PC: 0, PC: 1 */
          ,8,'m','u','l','t','i','p','l','e'
/* count=  2 Group:offset = EQ: 3, SP:22 */
          ,5,'n','a','m','e','s'
/* count=  2 Group:offset = EV: 1, MS:222 */
          ,7,'n','e','s','t','i','n','g'
/* count=  2 Group:offset = CO: 6, DO: 2 */
          ,13,'n','o','n','-','c','h','a','r','a'
              ,'c','t','e','r'
/* count=  2 Group:offset = CM: 2, IL:17 */
          ,8,'o','p','t','i','m','i','z','e'
/* count=  2 Group:offset = CP: 3, MS:227 */
          ,5,'o','t','h','e','r'
/* count=  2 Group:offset = EC: 1, IM: 4 */
          ,3,'o','u','t'
/* count=  2 Group:offset = MO: 1, SP:12 */
          ,7,'o','u','t','s','i','d','e'
/* count=  2 Group:offset = CM: 1, SP: 8 */
          ,9,'p','r','e','c','e','d','i','n','g'
/* count=  2 Group:offset = ST: 7, ST:17 */
          ,9,'p','r','e','c','i','s','i','o','n'
/* count=  2 Group:offset = MS:55, MS:209 */
          ,5,'q','u','o','t','e'
/* count=  2 Group:offset = FM:12, SX:20 */
          ,18,'r','e','c','u','r','s','i','o','n'
              ,' ','d','e','t','e','c','t','e','d'
/* count=  2 Group:offset = SP:26, SR: 8 */
          ,10,'r','e','f','e','r','e','n','c','e'
              ,'d'
/* count=  2 Group:offset = SR:11, ST: 5 */
          ,5,'r','e','l','a','x'
/* count=  2 Group:offset = MS:18, MS:19 */
          ,4,'s','a','m','e'
/* count=  2 Group:offset = LI: 6, SR: 9 */
          ,5,'s','a','v','e','d'
/* count=  2 Group:offset = SA: 0, SA: 1 */
          ,12,'s','o','u','r','c','e',' ','i','n'
              ,'p','u','t'
/* count=  2 Group:offset = CC: 0, CO: 5 */
          ,10,'s','t','a','t','e','m','e','n','t'
              ,'s'
/* count=  2 Group:offset = IM: 4, MS:126 */
          ,12,'s','u','b','s','t','r','i','n','g'
              ,'i','n','g'
/* count=  2 Group:offset = SS: 0, SS: 1 */
          ,6,'s','y','m','b','o','l'
/* count=  2 Group:offset = PR: 7, VA: 4 */
          ,6,'s','y','s','t','e','m'
/* count=  2 Group:offset = SM: 0, SM: 2 */
          ,4,'t','i','m','e'
/* count=  2 Group:offset = MS:58, MS:124 */
          ,5,'t','y','p','e','s'
/* count=  2 Group:offset = LI: 6, MD: 1 */
          ,12,'u','n','r','e','f','e','r','e','n'
              ,'c','e','d'
/* count=  2 Group:offset = VA: 4, MS: 8 */
          ,7,'w','a','r','n','i','n','g'
/* count=  2 Group:offset = MS: 8, MS: 9 */
          ,10,'w','i','l','l',' ','n','e','v','e'
              ,'r'
/* count=  2 Group:offset = ST: 7, ST:17 */
          ,2,'{','-'
/* count=  2 Group:offset = MS: 0 */
          ,1,'#'
/* count=  1 Group:offset = MS:45 */
          ,1,'$'
/* count=  1 Group:offset = FM: 7 */
          ,4,'%','d','1',','
/* count=  1 Group:offset = MS:116 */
          ,18,'*','A','p','p','l','i','c','a','t'
              ,'i','o','n',' ','T','y','p','e','*'
/* count=  1 Group:offset = MS:199 */
          ,13,'*','C','P','U',' ','T','a','r','g'
              ,'e','t','s','*'
/* count=  1 Group:offset = MS:168 */
          ,14,'*','C','h','a','r','a','c','t','e'
              ,'r',' ','S','e','t'
/* count=  1 Group:offset = MS:194 */
          ,8,'*','C','o','m','p','i','l','e'
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
          ,4,'.','E','Q','.'
/* count=  1 Group:offset = MD: 4 */
          ,4,'.','N','E','.'
/* count=  1 Group:offset = MD: 4 */
          ,21,'/','o','[','b','p',',','i',',','l'
              ,',','k',',','m',',','r',',','t',',','d'
              ,'o',']'
/* count=  1 Group:offset = MS:61 */
          ,14,'/','t','r','a','c','e',' ','/','b'
              ,'o','u','n','d','s'
/* count=  1 Group:offset = MS:16 */
          ,3,'1','-','5'
/* count=  1 Group:offset = CC: 4 */
          ,2,'1','9'
/* count=  1 Group:offset = CC: 5 */
          ,3,'2','/','1'
/* count=  1 Group:offset = MS:10 */
          ,3,'2','5','5'
/* count=  1 Group:offset = LI: 4 */
          ,3,'2','5','6'
/* count=  1 Group:offset = FM:13 */
          ,1,'4'
/* count=  1 Group:offset = IL: 3 */
          ,8,'5',' ','d','i','g','i','t','s'
/* count=  1 Group:offset = ST:19 */
          ,1,'6'
/* count=  1 Group:offset = VA: 1 */
          ,5,'6','5','5','3','5'
/* count=  1 Group:offset = SV: 8 */
          ,5,'9','9','9','9','9'
/* count=  1 Group:offset = ST:12 */
          ,11,'<','f','i','l','e','-','s','p','e'
              ,'c','>'
/* count=  1 Group:offset = MS: 0 */
          ,9,'A','L','L','O','C','A','T','E','D'
/* count=  1 Group:offset = LI:16 */
          ,4,'C','H','A','R'
/* count=  1 Group:offset = LI: 4 */
          ,7,'C','h','i','n','e','s','e'
/* count=  1 Group:offset = MS:195 */
          ,4,'C','o','d','e'
/* count=  1 Group:offset = MS:122 */
          ,8,'C','o','d','e','v','i','e','w'
/* count=  1 Group:offset = MS:215 */
          ,7,'C','o','m','p','i','l','e'
/* count=  1 Group:offset = MS:124 */
          ,1,'D'
/* count=  1 Group:offset = CC: 7 */
          ,15,'D','O','-','v','a','r','i','a','b'
              ,'l','e','s',' ','d','o'
/* count=  1 Group:offset = MS:218 */
          ,5,'D','W','A','R','F'
/* count=  1 Group:offset = MS:214 */
          ,25,'D','e','f','i','n','i','n','g',' '
              ,'s','u','b','p','r','o','g','r','a','m'
              ,':',' ','%','s','1',';'
/* count=  1 Group:offset = MS:114 */
          ,4,'E','L','S','E'
/* count=  1 Group:offset = IF: 0 */
          ,4,'E','N','D','='
/* count=  1 Group:offset = SP: 0 */
          ,5,'E','N','D','D','O'
/* count=  1 Group:offset = DO: 3 */
          ,17,'E','X','E','C','U','T','E',' ','u'
              ,'n','d','e','f','i','n','e','d'
/* count=  1 Group:offset = SP:13 */
          ,6,'E','w','.','d','D','e'
/* count=  1 Group:offset = FM: 2 */
          ,6,'E','w','.','d','Q','e'
/* count=  1 Group:offset = FM:20 */
          ,2,'F','F'
/* count=  1 Group:offset = MS:212 */
          ,4,'F','I','L','E'
/* count=  1 Group:offset = IL:12 */
          ,8,'F','U','N','C','T','I','O','N'
/* count=  1 Group:offset = SR: 6 */
          ,2,'I','F'
/* count=  1 Group:offset = IF: 0 */
          ,7,'I','N','T','E','G','E','R'
/* count=  1 Group:offset = SX:18 */
          ,15,'I','N','T','E','G','E','R','/','L'
              ,'O','G','I','C','A','L'
/* count=  1 Group:offset = MS:10 */
          ,7,'I','S','I','Z','E','O','F'
/* count=  1 Group:offset = LI:17 */
          ,3,'I','S','N'
/* count=  1 Group:offset = MS:114 */
          ,8,'J','a','p','a','n','e','s','e'
/* count=  1 Group:offset = MS:196 */
          ,6,'K','o','r','e','a','n'
/* count=  1 Group:offset = MS:197 */
          ,2,'L','F'
/* count=  1 Group:offset = MS:212 */
          ,4,'L','i','n','k'
/* count=  1 Group:offset = MS:169 */
          ,17,'N','A','M','E','L','I','S','T','-'
              ,'d','i','r','e','c','t','e','d'
/* count=  1 Group:offset = IL:16 */
          ,9,'N','O',' ','p','r','e','f','i','x'
/* count=  1 Group:offset = CO: 1 */
          ,4,'N','O','N','E'
/* count=  1 Group:offset = IM: 4 */
          ,4,'Q','w','.','d'
/* count=  1 Group:offset = FM:21 */
          ,4,'R','E','A','D'
/* count=  1 Group:offset = SP: 1 */
          ,3,'R','E','C'
/* count=  1 Group:offset = IL:14 */
          ,6,'S','E','L','E','C','T'
/* count=  1 Group:offset = SP: 5 */
          ,4,'T','H','E','N'
/* count=  1 Group:offset = IF: 1 */
          ,2,'T','O'
/* count=  1 Group:offset = GO: 2 */
          ,6,'U','s','a','g','e',':'
/* count=  1 Group:offset = MS: 0 */
          ,5,'V','A','L','U','E'
/* count=  1 Group:offset = PR: 1 */
          ,6,'W','A','T','C','O','M'
/* count=  1 Group:offset = MS:213 */
          ,1,'X'
/* count=  1 Group:offset = FM:19 */
          ,1,'Z'
/* count=  1 Group:offset = FM:15 */
          ,1,'\\'
/* count=  1 Group:offset = FM: 7 */
          ,6,'a','c','t','i','v','e'
/* count=  1 Group:offset = DO: 6 */
          ,6,'a','c','t','u','a','l'
/* count=  1 Group:offset = SF: 4 */
          ,18,'a','d','j','u','s','t','a','b','l'
              ,'e','/','a','s','s','u','m','e','d'
/* count=  1 Group:offset = SV: 1 */
          ,5,'a','l','i','g','n'
/* count=  1 Group:offset = MS:211 */
          ,3,'a','l','l'
/* count=  1 Group:offset = SP:11 */
          ,9,'a','l','l','o','c','a','t','e','d'
/* count=  1 Group:offset = MS:106 */
          ,5,'a','l','l','o','w'
/* count=  1 Group:offset = MS:224 */
          ,9,'a','l','t','e','r','n','a','t','e'
/* count=  1 Group:offset = RE: 0 */
          ,3,'a','n','y'
/* count=  1 Group:offset = SP: 4 */
          ,10,'a','p','o','s','t','r','o','p','h'
              ,'e'
/* count=  1 Group:offset = FM:12 */
          ,8,'a','p','p','e','a','r','e','d'
/* count=  1 Group:offset = SA: 2 */
          ,11,'a','r','g','u','m','e','n','t','('
              ,'s',')'
/* count=  1 Group:offset = LI:10 */
          ,13,'a','r','i','t','h','m','e','t','i'
              ,'c',' ','i','f'
/* count=  1 Group:offset = MS:112 */
          ,6,'a','r','r','a','y','s'
/* count=  1 Group:offset = SP:28 */
          ,8,'a','s','s','e','m','b','l','y'
/* count=  1 Group:offset = PR: 7 */
          ,6,'a','s','s','i','g','n'
/* count=  1 Group:offset = EQ: 1 */
          ,14,'a','s','s','i','g','n','e','d',' '
              ,'w','r','o','n','g'
/* count=  1 Group:offset = LI:14 */
          ,9,'a','t','t','e','m','p','t','e','d'
/* count=  1 Group:offset = SP:14 */
          ,9,'b','a','c','k','w','a','r','d','s'
/* count=  1 Group:offset = DO: 0 */
          ,3,'b','a','d'
/* count=  1 Group:offset = SX: 1 */
          ,14,'b','e','i','n','g',' ','i','n','c'
              ,'l','u','d','e','d'
/* count=  1 Group:offset = CO: 0 */
          ,7,'b','e','l','o','n','g','s'
/* count=  1 Group:offset = VA: 5 */
          ,6,'b','o','u','n','d','s'
/* count=  1 Group:offset = MS:193 */
          ,8,'b','r','a','n','c','h','e','d'
/* count=  1 Group:offset = ST: 2 */
          ,6,'b','r','o','w','s','e'
/* count=  1 Group:offset = MS:208 */
          ,8,'b','r','o','w','s','i','n','g'
/* count=  1 Group:offset = SM: 6 */
          ,5,'b','y','t','e','s'
/* count=  1 Group:offset = MO: 0 */
          ,7,'b','y','t','e','s',')',':'
/* count=  1 Group:offset = MS:122 */
          ,8,'c','a','l','l','-','r','e','t'
/* count=  1 Group:offset = MS:48 */
          ,12,'c','a','l','l','e','d',' ','s','i'
              ,'n','c','e'
/* count=  1 Group:offset = SR: 1 */
          ,8,'c','a','r','r','i','a','g','e'
/* count=  1 Group:offset = MS:201 */
          ,5,'c','a','s','e','s'
/* count=  1 Group:offset = SP: 6 */
          ,6,'c','a','u','s','e','d'
/* count=  1 Group:offset = EC: 0 */
          ,13,'c','h','a','r','a','c','t','e','r'
              ,'*','(','*',')'
/* count=  1 Group:offset = LI:16 */
          ,7,'c','o','l','u','m','n',','
/* count=  1 Group:offset = CC: 2 */
          ,8,'c','o','m','p','a','r','e','d'
/* count=  1 Group:offset = MD: 4 */
          ,12,'c','o','m','p','i','l','e','-','t'
              ,'i','m','e'
/* count=  1 Group:offset = MS:227 */
          ,9,'c','o','m','p','i','l','e','d',':'
/* count=  1 Group:offset = MS:126 */
          ,11,'c','o','n','d','i','t','i','o','n'
              ,'a','l'
/* count=  1 Group:offset = SP:29 */
          ,8,'c','o','n','t','i','n','u','e'
/* count=  1 Group:offset = CC: 2 */
          ,13,'c','o','u','n','t',' ','g','r','e'
              ,'a','t','e','r'
/* count=  1 Group:offset = FM:13 */
          ,13,'d','e','c','i','m','a','l',' ','p'
              ,'o','i','n','t'
/* count=  1 Group:offset = FM: 3 */
          ,12,'d','e','f','i','n','e',' ','m','a'
              ,'c','r','o'
/* count=  1 Group:offset = MS:17 */
          ,9,'d','e','l','i','m','i','t','e','r'
/* count=  1 Group:offset = FM: 0 */
          ,12,'d','e','p','e','n','d','e','n','c'
              ,'i','e','s'
/* count=  1 Group:offset = MS:198 */
          ,11,'d','e','s','c','r','i','p','t','o'
              ,'r',','
/* count=  1 Group:offset = FM:19 */
          ,11,'d','e','s','c','r','i','p','t','o'
              ,'r','s'
/* count=  1 Group:offset = MS:65 */
          ,7,'d','e','v','i','c','e','s'
/* count=  1 Group:offset = MS:201 */
          ,3,'d','i','d'
/* count=  1 Group:offset = GO: 0 */
          ,7,'d','i','s','a','b','l','e'
/* count=  1 Group:offset = MS:49 */
          ,4,'d','i','s','k'
/* count=  1 Group:offset = MS: 5 */
          ,18,'d','i','s','p','l','a','y',' ','d'
              ,'i','a','g','n','o','s','t','i','c'
/* count=  1 Group:offset = MS:12 */
          ,9,'d','u','p','l','i','c','a','t','e'
/* count=  1 Group:offset = SF: 0 */
          ,22,'d','y','n','a','m','i','c','a','l'
              ,'l','y',' ','a','l','l','o','c','a','t'
              ,'i','n','g'
/* count=  1 Group:offset = VA: 7 */
          ,4,'e','a','c','h'
/* count=  1 Group:offset = EC: 1 */
          ,11,'e','n','d','-','o','f','-','l','i'
              ,'n','e'
/* count=  1 Group:offset = CC: 6 */
          ,5,'e','q','u','a','l'
/* count=  1 Group:offset = EQ: 2 */
          ,11,'e','q','u','i','v','a','l','e','n'
              ,'c','e'
/* count=  1 Group:offset = CP:10 */
          ,7,'e','r','r','o','r','s',':'
/* count=  1 Group:offset = MS:123 */
          ,11,'e','s','t','a','b','l','i','s','h'
              ,'e','d'
/* count=  1 Group:offset = TY: 4 */
          ,5,'e','v','e','r','y'
/* count=  1 Group:offset = SA: 1 */
          ,9,'e','x','h','a','u','s','t','e','d'
/* count=  1 Group:offset = MO: 2 */
          ,5,'e','x','i','s','t'
/* count=  1 Group:offset = SM: 5 */
          ,7,'e','x','p','e','n','s','e'
/* count=  1 Group:offset = MS:227 */
          ,24,'e','x','p','l','i','c','i','t',' '
              ,'t','y','p','i','n','g',' ','r','e','q'
              ,'u','i','r','e','d'
/* count=  1 Group:offset = MS:13 */
          ,11,'e','x','t','e','n','s','i','o','n'
              ,'s',':'
/* count=  1 Group:offset = MS:127 */
          ,8,'e','x','t','e','r','n','a','l'
/* count=  1 Group:offset = MS:109 */
          ,6,'f','a','i','l','e','d'
/* count=  1 Group:offset = SM: 6 */
          ,6,'f','a','t','a','l',':'
/* count=  1 Group:offset = CP: 7 */
          ,6,'f','i','e','l','d','s'
/* count=  1 Group:offset = SP:28 */
          ,4,'f','i','n','d'
/* count=  1 Group:offset = SP: 3 */
          ,8,'f','o','l','l','o','w','e','d'
/* count=  1 Group:offset = SP: 5 */
          ,9,'f','o','l','l','o','w','i','n','g'
/* count=  1 Group:offset = CO: 2 */
          ,4,'f','o','r','m'
/* count=  1 Group:offset = DO: 4 */
          ,10,'f','o','r','m','a','t','/','F','M'
              ,'T'
/* count=  1 Group:offset = IL: 0 */
          ,11,'f','o','r','m','a','t','/','u','n'
              ,'i','t'
/* count=  1 Group:offset = SV: 7 */
          ,10,'f','o','r','m','a','t','t','i','n'
              ,'g'
/* count=  1 Group:offset = IL: 8 */
          ,5,'f','r','e','e','d'
/* count=  1 Group:offset = CP: 8 */
          ,15,'f','r','e','e','i','n','g',' ','u'
              ,'n','o','w','n','e','d'
/* count=  1 Group:offset = CP: 9 */
          ,4,'f','u','l','l'
/* count=  1 Group:offset = MS:46 */
          ,5,'f','u','l','l','y'
/* count=  1 Group:offset = CP: 3 */
          ,9,'f','u','n','c','t','i','o','n','s'
/* count=  1 Group:offset = MS:51 */
          ,10,'g','e','n','e','r','a','t','i','o'
              ,'n'
/* count=  1 Group:offset = SM: 6 */
          ,7,'g','e','n','e','r','i','c'
/* count=  1 Group:offset = LI:11 */
          ,4,'h','i','g','h'
/* count=  1 Group:offset = SP:10 */
          ,10,'i','m','p','l','i','c','i','t','l'
              ,'y'
/* count=  1 Group:offset = IM: 1 */
          ,12,'i','n','c','o','m','p','a','t','i'
              ,'b','l','e'
/* count=  1 Group:offset = CP: 1 */
          ,10,'i','n','c','o','m','p','l','e','t'
              ,'e'
/* count=  1 Group:offset = SP:19 */
          ,5,'i','n','f','o','.'
/* count=  1 Group:offset = MS:215 */
          ,12,'i','n','i','t','i','a','l','i','z'
              ,'i','n','g'
/* count=  1 Group:offset = CM: 1 */
          ,6,'i','n','s','i','d','e'
/* count=  1 Group:offset = SP: 9 */
          ,22,'i','n','s','t','r','u','c','t','i'
              ,'o','n',' ','s','c','h','e','d','u','l'
              ,'i','n','g'
/* count=  1 Group:offset = MS:56 */
          ,11,'i','n','t','e','r','r','u','p','t'
              ,'e','d'
/* count=  1 Group:offset = KO: 4 */
          ,9,'i','n','v','a','r','i','a','n','t'
/* count=  1 Group:offset = MS:229 */
          ,6,'i','t','s','e','l','f'
/* count=  1 Group:offset = ST:14 */
          ,8,'k','e','y','b','o','a','r','d'
/* count=  1 Group:offset = KO: 4 */
          ,7,'k','e','y','w','o','r','d'
/* count=  1 Group:offset = ST:18 */
          ,4,'l','a','s','t'
/* count=  1 Group:offset = IF: 0 */
          ,4,'l','e','f','t'
/* count=  1 Group:offset = EC: 0 */
          ,4,'l','e','s','s'
/* count=  1 Group:offset = ST:12 */
          ,6,'l','e','t','t','e','r'
/* count=  1 Group:offset = IM: 1 */
          ,5,'l','e','v','e','l'
/* count=  1 Group:offset = CO: 6 */
          ,9,'l','i','b','r','a','r','i','e','s'
/* count=  1 Group:offset = MS:37 */
          ,4,'l','i','n','k'
/* count=  1 Group:offset = MS:81 */
          ,7,'l','i','t','e','r','a','l'
/* count=  1 Group:offset = SX:20 */
          ,6,'m','a','n','g','l','e'
/* count=  1 Group:offset = MS:222 */
          ,4,'m','a','t','h'
/* count=  1 Group:offset = MS:53 */
          ,6,'m','e','m','b','e','r'
/* count=  1 Group:offset = MS:115 */
          ,11,'m','i','s','m','a','t','c','h','i'
              ,'n','g'
/* count=  1 Group:offset = CO: 7 */
          ,15,'m','i','s','s','p','e','l','l','e'
              ,'d',' ','w','o','r','d'
/* count=  1 Group:offset = ST:16 */
          ,6,'m','i','x','i','n','g'
/* count=  1 Group:offset = MD: 1 */
          ,8,'m','o','d','i','f','i','e','d'
/* count=  1 Group:offset = CP: 5 */
          ,8,'m','o','d','i','f','i','e','r'
/* count=  1 Group:offset = FM: 8 */
          ,4,'m','o','v','e'
/* count=  1 Group:offset = MS:228 */
          ,11,'m','u','l','t','i','t','h','r','e'
              ,'a','d'
/* count=  1 Group:offset = MS:79 */
          ,5,'n','a','m','e','d'
/* count=  1 Group:offset = CM: 4 */
          ,10,'n','e','a','r',' ','\'','%','s','2'
              ,'\''
/* count=  1 Group:offset = PR: 4 */
          ,13,'n','o','n','-','C','H','A','R','A'
              ,'C','T','E','R'
/* count=  1 Group:offset = EV: 4 */
          ,7,'n','u','m','e','r','i','c'
/* count=  1 Group:offset = LI: 7 */
          ,9,'n','u','m','e','r','i','c','a','l'
/* count=  1 Group:offset = MS:54 */
          ,6,'o','b','j','e','c','t'
/* count=  1 Group:offset = MS:67 */
          ,7,'o','p','e','r','a','n','d'
/* count=  1 Group:offset = MD: 0 */
          ,15,'o','p','e','r','a','t','e',' ','q'
              ,'u','i','e','t','l','y'
/* count=  1 Group:offset = MS:202 */
          ,9,'o','p','e','r','a','t','o','r','s'
/* count=  1 Group:offset = SX: 1 */
          ,20,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s','/','a','s','s','u','m'
              ,'e'
/* count=  1 Group:offset = MS:229 */
          ,23,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s','/','u','n','r','o','l'
              ,'l','i','n','g'
/* count=  1 Group:offset = MS:210 */
          ,11,'o','v','e','r','l','a','p','p','i'
              ,'n','g'
/* count=  1 Group:offset = SP: 6 */
          ,4,'p','a','t','h'
/* count=  1 Group:offset = MS:219 */
          ,3,'p','e','r'
/* count=  1 Group:offset = SV: 8 */
          ,9,'p','e','r','m','i','t','t','e','d'
/* count=  1 Group:offset = BD: 1 */
          ,8,'p','o','s','i','t','i','o','n'
/* count=  1 Group:offset = ST:20 */
          ,8,'p','o','s','i','t','i','v','e'
/* count=  1 Group:offset = DA: 1 */
          ,10,'p','r','e','d','i','c','t','i','o'
              ,'n'
/* count=  1 Group:offset = MS:221 */
          ,8,'p','r','e','v','i','o','u','s'
/* count=  1 Group:offset = SA: 2 */
          ,7,'p','r','i','n','t','e','r'
/* count=  1 Group:offset = MS: 3 */
          ,7,'p','r','o','m','o','t','e'
/* count=  1 Group:offset = MS:223 */
          ,8,'p','r','o','p','e','r','l','y'
/* count=  1 Group:offset = SA: 0 */
          ,10,'q','u','a','n','t','i','t','i','e'
              ,'s'
/* count=  1 Group:offset = MD: 4 */
          ,7,'r','e','a','d','i','n','g'
/* count=  1 Group:offset = SM: 0 */
          ,10,'r','e','c','o','g','n','i','z','e'
              ,'d'
/* count=  1 Group:offset = CO: 4 */
          ,11,'r','e','c','u','r','s','i','v','e'
              ,'l','y'
/* count=  1 Group:offset = SP:14 */
          ,15,'r','e','d','e','f','i','n','e','d'
              ,' ','w','h','i','l','e'
/* count=  1 Group:offset = DO: 6 */
          ,10,'r','e','l','a','t','i','o','n','a'
              ,'l'
/* count=  1 Group:offset = MD: 0 */
          ,18,'r','e','l','a','t','i','v','e',' '
              ,'p','o','s','i','t','i','o','n','s'
/* count=  1 Group:offset = EV: 0 */
          ,25,'r','e','q','u','i','r','e',' ','s'
              ,'e','q','u','e','n','t','i','a','l',' '
              ,'a','c','c','e','s','s'
/* count=  1 Group:offset = IL:13 */
          ,8,'r','e','s','o','l','v','e','d'
/* count=  1 Group:offset = PR: 7 */
          ,8,'r','e','s','o','u','r','c','e'
/* count=  1 Group:offset = MS:200 */
          ,5,'r','i','g','h','t'
/* count=  1 Group:offset = SX: 3 */
          ,18,'r','u','n','-','t','i','m','e',' '
              ,'t','r','a','c','e','b','a','c','k'
/* count=  1 Group:offset = MS:192 */
          ,21,'s','a','v','e','s',' ','i','n','t'
              ,'o',' ','f','l','o','w',' ','g','r','a'
              ,'p','h'
/* count=  1 Group:offset = MS:228 */
          ,9,'s','e','c','o','n','d','s',')',':'
/* count=  1 Group:offset = MS:124 */
          ,7,'s','e','g','m','e','n','t'
/* count=  1 Group:offset = MS:222 */
          ,8,'s','e','g','m','e','n','t','s'
/* count=  1 Group:offset = MS:211 */
          ,6,'s','e','l','e','c','t'
/* count=  1 Group:offset = SP: 7 */
          ,9,'s','e','p','a','r','a','t','o','r'
/* count=  1 Group:offset = MS:224 */
          ,6,'s','i','m','p','l','e'
/* count=  1 Group:offset = MS:100 */
          ,5,'s','l','a','s','h'
/* count=  1 Group:offset = SX:14 */
          ,5,'s','p','a','c','e'
/* count=  1 Group:offset = MS:57 */
          ,9,'s','p','e','c','i','f','i','e','d'
/* count=  1 Group:offset = PR: 1 */
          ,7,'s','p','e','c','i','f','y'
/* count=  1 Group:offset = IL: 5 */
          ,5,'s','t','a','c','k'
/* count=  1 Group:offset = MS:64 */
          ,10,'s','t','a','t','e','m','e','n','t'
              ,','
/* count=  1 Group:offset = MS:113 */
          ,63,'s','t','a','t','e','m','e','n','t'
              ,'s',',',' ','%','i','2',' ','b','y','t'
              ,'e','s',',',' ','%','d','3',' ','e','x'
              ,'t','e','n','s','i','o','n','s',',',' '
              ,'%','d','4',' ','w','a','r','n','i','n'
              ,'g','s',',',' ','%','d','5',' ','e','r'
              ,'r','o','r','s'
/* count=  1 Group:offset = MS:121 */
          ,6,'s','t','r','e','a','m'
/* count=  1 Group:offset = CO: 5 */
          ,7,'s','t','r','i','n','g',','
/* count=  1 Group:offset = FM: 0 */
          ,10,'s','t','r','u','c','t','u','r','e'
              ,'d'
/* count=  1 Group:offset = EY: 1 */
          ,10,'s','t','r','u','c','t','u','r','e'
              ,'s'
/* count=  1 Group:offset = SP:28 */
          ,9,'s','u','b','s','c','r','i','p','t'
/* count=  1 Group:offset = SV: 2 */
          ,11,'s','u','b','s','c','r','i','p','t'
              ,'e','d'
/* count=  1 Group:offset = EV: 2 */
          ,10,'s','u','b','s','c','r','i','p','t'
              ,'s'
/* count=  1 Group:offset = SV: 3 */
          ,4,'s','u','c','h'
/* count=  1 Group:offset = SP:21 */
          ,12,'s','y','n','t','a','x',' ','c','h'
              ,'e','c','k'
/* count=  1 Group:offset = MS:36 */
          ,4,'t','a','k','e'
/* count=  1 Group:offset = CO: 1 */
          ,6,'t','a','r','g','e','t'
/* count=  1 Group:offset = EQ: 0 */
          ,9,'t','e','m','p','o','r','a','r','y'
/* count=  1 Group:offset = SM: 5 */
          ,8,'t','e','r','m','i','n','a','l'
/* count=  1 Group:offset = MS: 4 */
          ,9,'t','e','r','m','i','n','a','t','e'
/* count=  1 Group:offset = DO: 3 */
          ,11,'t','e','r','m','i','n','a','t','i'
              ,'o','n'
/* count=  1 Group:offset = DO: 1 */
          ,4,'t','e','x','t'
/* count=  1 Group:offset = FM: 9 */
          ,9,'t','h','r','e','s','h','o','l','d'
/* count=  1 Group:offset = MS:63 */
          ,7,'t','r','e','a','t','e','d'
/* count=  1 Group:offset = SV: 6 */
          ,3,'t','w','o'
/* count=  1 Group:offset = SR: 9 */
          ,6,'u','n','a','b','l','e'
/* count=  1 Group:offset = CP: 3 */
          ,5,'u','n','a','r','y'
/* count=  1 Group:offset = MD: 5 */
          ,12,'u','n','d','e','c','o','d','e','a'
              ,'b','l','e'
/* count=  1 Group:offset = ST:16 */
          ,9,'u','n','d','e','r','f','l','o','w'
/* count=  1 Group:offset = KO: 2 */
          ,10,'u','n','f','i','n','i','s','h','e'
              ,'d'
/* count=  1 Group:offset = SP:17 */
          ,5,'u','n','i','o','n'
/* count=  1 Group:offset = SP:27 */
          ,5,'u','n','i','t','s'
/* count=  1 Group:offset = SR: 9 */
          ,21,'u','n','m','a','t','c','h','e','d'
              ,' ','p','a','r','e','n','t','h','e','s'
              ,'e','s'
/* count=  1 Group:offset = PC: 3 */
          ,7,'u','n','n','a','m','e','d'
/* count=  1 Group:offset = SR:10 */
          ,12,'u','s','e','r','-','d','e','f','i'
              ,'n','e','d'
/* count=  1 Group:offset = SF: 6 */
          ,5,'v','a','l','i','d'
/* count=  1 Group:offset = SS: 1 */
          ,9,'w','a','r','n','i','n','g','s',':'
/* count=  1 Group:offset = MS:125 */
          ,8,'w','h','i','c','h',' ','i','t'
/* count=  1 Group:offset = SA: 1 */
          ,5,'w','i','d','t','h'
/* count=  1 Group:offset = FM:14 */
          ,4,'w','i','l','d'
/* count=  1 Group:offset = MS:19 */
          ,8,'w','i','n','d','o','w','e','d'
/* count=  1 Group:offset = MS:77 */
          ,7,'w','i','t','h','o','u','t'
/* count=  1 Group:offset = SV: 4 */
          ,7,'w','r','i','t','i','n','g'
/* count=  1 Group:offset = SM: 2 */
          ,20,'z','e','r','o','*','*','J',' ','w'
              ,'h','e','r','e',' ','J',' ','<','=',' '
              ,'0'
/* count=  1 Group:offset = EX: 0 */
                   };
/* Total number of phrases = 561 */
