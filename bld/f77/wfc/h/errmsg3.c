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
 8,21,27,5,95,8,37,12,0
/* dummy argument %s1 appears more than once  */
,7,76,17,0,73,104,78,88
,0
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
,11,76,17,0,14,1,80,117,53,5,208,11
};
static const char __FAR Msg256[] = {
/* %s1 was initialized in a block data subprogram but is not in COMMON  */
 13,0,65,127,3,9,30,66,50,45,4,1,3,23
/* %s1 statement is not permitted in a BLOCK DATA subprogram  */
,12,0,2,4,1,255,255,11,3,9,71,151,50
};
static const char __FAR Msg512[] = {
/* invalid character encountered in source input  */
 6,21,24,239,3,255,35
/* invalid character in statement number columns  */
,6,21,24,3,2,27,228
/* character in continuation column, but no statement to continue  */
,11,24,3,98,255,164,45,70,2,8,255,170
/* character encountered is not FORTRAN 77 standard  */
,5,24,239,4,1,6
/* columns 1-5 in a continuation line must be blank  */
,10,228,255,64,3,9,98,31,14,7,157
/* more than 19 continuation lines is not FORTRAN 77 standard  */
,10,104,78,255,65,98,255,12,4,1,6
/* end-of-line comment is not FORTRAN 77 standard  */
,6,255,188,229,4,1,6
/* D in column 1 is not FORTRAN 77 standard  */
,8,255,84,3,161,146,4,1,6
/* too many continuation lines  */
,5,110,180,98,255,12
};
static const char __FAR Msg768[] = {
/* %s1 already in COMMON  */
 4,0,59,3,23
/* initializing %s1 in COMMON outside of block data subprogram is not FORTRAN 77 standard  */
,14,255,225,0,3,23,255,26,5,30,66,50,4,1,6
/* character and non-character data in COMMON is not FORTRAN 77 standard  */
,10,24,51,255,21,66,3,23,4,1,6
/* COMMON block %s1 has been defined with a different size  */
,10,23,30,0,25,39,52,29,9,166,40
/* named COMMON block %s1 appears in more than one BLOCK DATA subprogram  */
,13,255,253,23,30,0,73,3,104,78,132,71,151,50
/* blank COMMON block has been defined with a different size  */
,10,157,23,30,25,39,52,29,9,166,40
};
static const char __FAR Msg1024[] = {
/* DOUBLE PRECISION COMPLEX constants are not FORTRAN 77 standard  */
 6,112,206,97,81,1,6
/* invalid floating-point constant %s1  */
,4,21,101,35,0
/* zero length character constants are not allowed  */
,7,111,47,24,97,81,1,19
/* invalid hexadecimal/octal constant  */
,3,21,254,35
/* hexadecimal/octal constant is not FORTRAN 77 standard  */
,5,254,35,4,1,6
};
static const char __FAR Msg1280[] = {
/* %s1 is already being included  */
 5,0,4,59,255,148
/* '%s1' option cannot take a NO prefix  */
,9,79,106,16,255,255,56,9,255,111
/* expecting an equals sign following the %s1 option  */
,9,10,15,242,190,255,206,28,0,106
/* the '%s1' option requires a number  */
,6,28,79,106,136,9,27
/* option '%s1' not recognized - ignored  */
,9,106,79,1,255,255,21,58,255,3
/* '%s1' option not allowed in source input stream  */
,10,79,106,1,19,3,255,35,255,255,47
/* nesting level exceeded for compiler directives  */
,8,255,20,255,238,170,20,121,236
/* mismatching compiler directives  */
,4,255,246,121,236
};
static const char __FAR Msg1536[] = {
 0
/* argument %d1 incompatible with register  */
,6,17,143,255,222,29,135
/* subprogram %s1 has invalid return register  */
,6,50,0,25,21,137,135
/* low on memory - unable to fully optimize %s1  */
,14,255,16,87,55,58,255,255,66,8,255,215,255,23,0
/* internal compiler error %d1  */
,4,129,121,82,143
/* illegal register modified by %s1  */
,6,36,135,255,249,118,0
/* %s1  */
,1,0
/* fatal: %s1  */
,3,255,201,0
/* dynamic memory not freed  */
,5,122,55,1,255,212
/* freeing unowned dynamic memory  */
,4,255,213,122,55
/* The automatic equivalence containing %s1 exceeds 32K limit  */
,9,152,156,255,190,231,0,100,148,86
/* The return value of %s1 exceeds 32K limit  */
,8,152,137,197,5,0,100,148,86
/* The automatic variable %s1 exceeds 32K limit  */
,7,152,156,22,0,100,148,86
};
static const char __FAR Msg1792[] = {
/* CHARACTER variable %s1 with length (*) not allowed in this expression  */
 11,150,22,0,29,47,91,1,19,3,57,67
/* character variable %s1 with length (*) must be a subprogram argument  */
,11,24,22,0,29,47,91,14,7,9,50,17
};
static const char __FAR Msg2048[] = {
/* implied DO variable %s1 must be an integer variable  */
 9,172,43,22,0,14,7,15,69,22
/* repeat specification must be a positive integer  */
,9,187,49,14,7,9,255,255,13,69
/* %s1 appears in an expression but is not an implied DO variable  */
,12,0,73,3,15,67,45,4,1,15,172,43,22
/* %s1 in blank COMMON block cannot be initialized  */
,8,0,3,157,23,30,16,7,127
/* data initialization with hexadecimal constant is not FORTRAN 77 standard  */
,9,66,255,7,29,253,35,4,1,6
/* cannot initialize %s1 %s2  */
,5,16,255,8,0,11
/* data initialization in %s1 statement is not FORTRAN 77 standard  */
,9,66,255,7,3,0,2,4,1,6
/* not enough constants for list of variables  */
,7,1,240,97,20,48,5,90
,0
/* too many constants for list of variables  */
,7,110,180,97,20,48,5,90
,0
/* cannot initialize %s1 variable %s2 with %s3 constant  */
,9,16,255,8,0,22,11,29,144,35
};
static const char __FAR Msg2304[] = {
/* using %s1 incorrectly in dimension expression  */
 6,196,0,173,3,235,67
};
static const char __FAR Msg2560[] = {
/* statement number %i1 already defined in line %d2 - DO loop is backwards  */
 14,2,27,42,59,52,3,31,41,58,43,54,4,255,145
/* %s1 statement not allowed at termination of DO range  */
,11,0,2,1,19,74,255,255,61,5,43,108
/* improper nesting of DO loop  */
,7,255,5,255,20,5,43,54
/* ENDDO cannot terminate DO loop with statement label  */
,11,255,90,16,255,255,60,43,54,29,2,176
/* this DO loop form is not FORTRAN 77 standard  */
,8,57,43,54,255,207,4,1,6
/* expecting comma or DO variable  */
,5,10,120,13,43,22
/* DO variable cannot be redefined while DO loop is active  */
,12,43,22,16,7,255,255,23,43,54,4,255,126
};
static const char __FAR Msg2816[] = {
/* equivalencing %s1 has caused extension of COMMON block %s2 to the left  */
 14,243,0,25,255,162,247,5,23,30,11,8,28,255,235
/* %s1 and %s2 in COMMON are equivalenced to each other  */
,12,0,51,11,3,23,81,169,8,255,185,255,24
};
static const char __FAR Msg3072[] = {
/* missing END statement  */
 3,26,207,2
};
static const char __FAR Msg3328[] = {
/* target of assignment is illegal  */
 7,255,255,57,5,154,4,36
/* cannot assign value to %s1  */
,6,16,255,141,197,8,0
/* illegal use of equal sign  */
,6,36,141,5,255,189,190
/* multiple assignment is not FORTRAN 77 standard  */
,6,255,18,154,4,1,6
/* expecting equals sign  */
,3,10,242,190
};
static const char __FAR Msg3584[] = {
/* %s1 has been equivalenced to 2 different relative positions  */
 10,0,25,39,169,8,147,166,255,255,26
/* EQUIVALENCE list must contain at least 2 names  */
,9,209,48,14,162,74,177,147,255,19
/* %s1 incorrectly subscripted in %s2 statement  */
,8,0,173,255,255,52,3,11,2
/* incorrect substring of %s1 in %s2 statement  */
,8,255,6,192,5,0,3,11,2
/* equivalencing CHARACTER and non-CHARACTER data is not FORTRAN 77 standard  */
,10,243,150,51,255,255,0,66,4,1,6
/* attempt to substring %s1 in EQUIVALENCE statement but type is %s2  */
,11,116,8,192,0,3,209,2,45,34,4,11
};
static const char __FAR Msg3840[] = {
/* zero**J where J <= 0 is not allowed  */
 6,255,255,84,4,1,19
};
static const char __FAR Msg4096[] = {
/* type of entry %s1 does not match type of function %s2  */
 11,34,5,241,0,167,1,181,34,5,12,11
/* ENTRY statement not allowed within structured control blocks  */
,10,208,2,1,19,142,255,255,49,75,223
/* size of entry %s1 does not match size of function %s2  */
,11,40,5,241,0,167,1,181,40,5,12,11
};
static const char __FAR Msg4352[] = {
/* missing delimiter in format string, comma assumed  */
 10,26,255,174,3,46,255,255,48,120,115
/* missing or invalid constant  */
,4,26,13,21,35
/* Ew.dDe format code is not FORTRAN 77 standard  */
,7,255,93,46,61,4,1,6
/* missing decimal point  */
,3,26,255,172
/* missing or invalid edit descriptor in format string  */
,8,26,13,21,123,165,3,46,191
/* unrecognizable edit descriptor in format string  */
,6,195,123,165,3,46,191
/* invalid repeat specification  */
,3,21,187,49
/* $ or \ format code is not FORTRAN 77 standard  */
,10,255,47,13,255,125,46,61,4,1,6
/* invalid field modifier  */
,4,21,83,255,250
/* expecting end of FORMAT statement but found more text  */
,11,10,77,5,92,2,45,102,104,255,255,62
/* repeat specification not allowed for this format code  */
,8,187,49,1,19,20,57,46,61
/* no statement number on FORMAT statement  */
,6,70,2,27,87,92,2
/* no closing quote on apostrophe edit descriptor  */
,9,70,160,255,29,87,255,135,123,165
/* field count greater than 256 is invalid  */
,8,83,255,171,78,255,68,4,21
/* invalid field width specification  */
,6,21,83,255,255,79,49
/* Z format code is not FORTRAN 77 standard  */
,7,255,124,46,61,4,1,6
,0
,0
,0
/* missing constant before X edit descriptor, 1 assumed  */
,10,26,35,117,255,123,123,255,176,146,115
/* Ew.dQe format code is not FORTRAN 77 standard  */
,7,255,94,46,61,4,1,6
/* Qw.d format code is not FORTRAN 77 standard  */
,7,255,113,46,61,4,1,6
};
static const char __FAR Msg4608[] = {
/* %s1 statement label may not appear in ASSIGN statement but did in line %d2  */
 15,0,2,176,182,1,80,3,203,2,45,255,179,3,31,41
/* ASSIGN of statement number %i1 in line %d2 not allowed  */
,10,203,5,2,27,42,3,31,41,1,19
/* expecting TO  */
,3,10,255,118
};
static const char __FAR Msg4864[] = {
/* hollerith constant is not FORTRAN 77 standard  */
 6,255,0,35,4,1,6
/* not enough characters for hollerith constant  */
,7,1,240,159,20,255,0,35
};
static const char __FAR Msg5120[] = {
/* ELSE block must be the last block in block IF  */
 13,255,88,30,14,7,28,255,234,30,3,30,255,101
/* expecting THEN  */
,3,10,255,117
};
static const char __FAR Msg5376[] = {
/* missing or invalid format/FMT specification  */
 6,26,13,21,255,208,49
/* the UNIT may not be an internal file for this statement  */
,11,28,218,182,1,7,15,129,32,20,57,2
/* %s1 statement cannot have %s2 specification  */
,6,0,2,16,125,11,49
/* variable must have a size of 4  */
,8,22,14,125,9,40,5,255,69
/* missing or unrecognizable control list item %s1  */
,7,26,13,195,75,48,175,0
/* attempt to specify control list item %s1 more than once  */
,12,116,8,255,255,44,75,48,175,0,104,78,88
/* implied DO loop has no input/output list  */
,8,172,43,54,25,70,255,9,48
/* list-directed input/output with internal files is not FORTRAN 77 standard  */
,10,255,13,255,9,29,129,84,4,1,6
/* FORTRAN 77 standard requires an asterisk for list-directed formatting  */
,9,6,136,15,155,20,255,13,255,210
/* missing or improper unit identification  */
,7,26,13,255,5,194,255,1
/* missing unit identification or file specification  */
,7,26,194,255,1,13,32,49
/* asterisk unit identifier not allowed in %s1 statement  */
,9,155,194,255,2,1,19,3,0,2
/* cannot have both UNIT and FILE specifier  */
,8,16,125,158,218,51,255,97,64
/* internal files require sequential access  */
,5,129,84,255,255,27
/* END specifier with REC specifier is not FORTRAN 77 standard  */
,9,207,64,29,255,115,64,4,1,6
/* %s1 specifier in i/o list is not FORTRAN 77 standard  */
,8,0,64,3,171,48,4,1,6
/* i/o list is not allowed with NAMELIST-directed format  */
,9,171,48,4,1,19,29,255,110,46
/* non-character array as format specifier is not FORTRAN 77 standard  */
,9,255,21,38,44,46,64,4,1,6
};
static const char __FAR Msg5632[] = {
/* illegal range of characters  */
 4,36,108,5,159
/* letter can only be implicitly declared once  */
,9,255,237,96,62,7,255,221,163,88
/* unrecognizable type  */
,2,195,34
/* (*) length specifier in an IMPLICIT statement is not FORTRAN 77 standard  */
,10,91,47,64,3,15,113,2,4,1,6
/* IMPLICIT NONE allowed once or not allowed with other IMPLICIT statements  */
,14,113,255,112,19,88,13,1,19,29,255,24,113,255,36
};
static const char __FAR Msg6144[] = {
/* floating-point divide by zero  */
 4,101,237,118,111
/* floating-point overflow  */
,2,101,185
/* floating-point underflow  */
,4,101,255,255,69
/* integer divide by zero  */
,4,69,237,118,111
/* program interrupted from keyboard  */
,6,107,255,228,124,255,231
/* integer overflow  */
,2,69,185
};
static const char __FAR Msg6400[] = {
 0
,0
,0
,0
/* argument of CHAR must be in the range zero to 255  */
,13,17,5,255,79,14,7,3,28,108,111,8,255,67
/* %s1 intrinsic function cannot be passed 2 complex arguments  */
,9,0,37,12,16,7,134,147,230,95
/* argument types must be the same for the %s1 intrinsic function  */
,13,17,255,41,14,7,28,255,33,20,28,0,37,12
/* expecting numeric argument, but %s1 argument was found  */
,10,10,255,255,1,221,45,0,17,65,102
,0
/* cannot pass %s1 as argument to intrinsic function  */
,8,16,186,0,44,17,8,37,12
/* intrinsic function requires argument(s)  */
,5,37,12,136,255,137
/* %s1 argument type is invalid for this generic function  */
,10,0,17,34,4,21,20,57,255,218,12
/* this intrinsic function cannot be passed as an argument  */
,9,57,37,12,16,7,134,44,15,17
/* expecting %s1 argument, but %s2 argument was found  */
,8,10,0,221,45,11,17,65,102
/* intrinsic function was assigned wrong type  */
,6,37,12,65,255,142,34
/* intrinsic function %s1 is not FORTRAN 77 standard  */
,6,37,12,0,4,1,6
/* argument to ALLOCATED intrinsic function must be an allocatable array or character*(*) variable  */
,15,17,8,255,78,37,12,14,7,15,219,38,13,255,163,22
/* invalid argument to ISIZEOF intrinsic function  */
,7,21,17,8,255,104,37,12
};
static const char __FAR Msg6656[] = {
/* relational operator has a logical operand  */
 10,255,255,25,63,25,9,178,255,255,3
/* mixing DOUBLE PRECISION and COMPLEX types is not FORTRAN 77 standard  */
,10,255,248,112,51,206,255,41,4,1,6
/* operator not expecting %s1 operands  */
,5,63,1,10,0,184
/* operator not expecting %s1 and %s2 operands  */
,7,63,1,10,0,51,11,184
/* complex quantities can only be compared using .EQ. or .NE.  */
,15,230,255,255,19,96,62,7,255,166,196,255,60,13,255,61
/* unary operator not expecting %s1 type  */
,8,255,255,67,63,1,10,0,34
/* logical operator with integer operands is not FORTRAN 77 standard  */
,8,178,63,29,69,184,4,1,6
/* logical operator %s1 is not FORTRAN 77 standard  */
,6,178,63,0,4,1,6
};
static const char __FAR Msg6912[] = {
/* %s1 exceeds compiler limit of %u2 bytes  */
 8,0,100,121,86,5,199,255,154
/* out of memory  */
,4,255,25,5,55
/* dynamic memory exhausted due to length of this statement - statement ignored  */
,14,122,55,255,194,168,8,47,5,57,2,58,2,255,3
};
static const char __FAR Msg7168[] = {
/* missing or misplaced closing parenthesis  */
 6,26,13,255,17,160,133
/* missing or misplaced opening parenthesis  */
,6,26,13,255,17,183,133
/* unexpected parenthesis  */
,2,193,133
/* unmatched parentheses  */
,3,255,255,73
};
static const char __FAR Msg7424[] = {
/* expecting symbolic name  */
 3,10,140,18
/* illegal size specified for VALUE attribute  */
,9,36,40,255,255,43,20,255,120,222
/* illegal argument attribute  */
,3,36,17,222
/* continuation line must contain a comment character in column 1  */
,10,98,31,14,162,9,229,24,3,161,146
/* expecting '%s1' near '%s2'  */
,4,10,79,255,254
/* in-line byte sequence limit exceeded  */
,5,126,224,189,86,170
/* illegal hexadecimal data in byte sequence  */
,6,36,253,66,3,224,189
/* symbol '%s1' in in-line assembly code cannot be resolved  */
,13,255,38,79,3,126,255,140,61,16,7,255,255,28
};
static const char __FAR Msg7680[] = {
/* alternate return specifier only allowed in subroutine  */
 8,255,133,137,64,62,19,3,139
/* RETURN statement in main program is not FORTRAN 77 standard  */
,8,216,2,3,179,107,4,1,6
};
static const char __FAR Msg7936[] = {
/* COMMON block %s1 saved but not properly defined  */
 11,23,30,0,255,34,45,1,255,255,18,52
/* COMMON block %s1 must be saved in every subprogram in which it appears  */
,16,23,30,0,14,7,255,34,3,255,193,50,3,255,255,78,73
/* name already appeared in a previous SAVE statement  */
,11,18,59,255,136,3,9,255,255,15,217,2
};
static const char __FAR Msg8192[] = {
/* statement function definition contains duplicate dummy arguments  */
 8,2,12,53,232,255,183,76,95
/* character length of statement function name must not be (*)  */
,10,24,47,5,2,12,18,14,1,7,91
/* statement function definition contains illegal dummy argument  */
,7,2,12,53,232,36,76,17
/* cannot pass %s1 %s2 to statement function  */
,7,16,186,0,11,8,2,12
/* %s1 actual argument was passed to %s2 dummy argument  */
,10,0,255,127,17,65,134,8,11,76,17
/* incorrect number of arguments passed to statement function %s1  */
,10,255,6,27,5,95,134,8,2,12,0
/* type of statement function name must not be a user-defined structure  */
,13,34,5,2,12,18,14,1,7,9,255,255,75,33
};
static const char __FAR Msg8448[] = {
/* system file error reading %s1 - %s2  */
 10,255,39,32,82,255,255,20,0,58,11
/* error opening file %s1 - %s2  */
,6,82,183,32,0,58,11
/* system file error writing %s1 - %s2  */
,10,255,39,32,82,255,255,83,0,58,11
,0
,0
/* error opening %s1 - too many temporary files exist  */
,12,82,183,0,58,110,180,255,255,58,84,255,195
/* generation of browsing information failed  */
,8,255,217,5,255,153,85,255,200
};
static const char __FAR Msg8704[] = {
/* cannot have both ATEND and the END= specifier  */
 9,16,125,158,204,51,28,255,89,64
/* ATEND must immediately follow a READ statement  */
,9,204,14,255,4,251,9,255,114,2
/* block label must be a symbolic name  */
,7,30,176,14,7,9,140,18
/* could not find a structure to %s1 from  */
,9,233,1,255,203,9,33,8,0,124
/* REMOTE BLOCK is not allowed in the range of any control structure  */
,13,94,71,4,1,19,3,28,108,5,255,134,75,33
/* the SELECT statement must be followed immediately by a CASE statement  */
,14,28,255,116,2,14,7,255,205,255,4,118,9,205,2
/* cases are overlapping  */
,6,255,161,81,255,255,8
/* select structure requires at least one CASE statement  */
,10,255,255,37,33,136,74,177,132,205,2
/* cannot branch to %i1 from outside control structure in line %d2  */
,12,16,60,8,42,124,255,26,75,33,3,31,41
/* cannot branch to %i1 inside structure on line %d2  */
,10,16,60,8,42,255,226,33,87,31,41
/* low end of range exceeds the high end  */
,10,255,16,77,5,108,100,28,255,220,77
/* default case block must follow all case blocks  */
,9,164,225,30,14,251,255,130,225,223
/* attempt to branch out of a REMOTE BLOCK  */
,9,116,8,60,255,25,5,9,94,71
/* attempt to EXECUTE undefined REMOTE BLOCK %s1  */
,7,116,8,255,91,94,71,0
/* attempted to use REMOTE BLOCK recursively  */
,9,255,144,8,141,94,71,255,255,22
/* cannot RETURN from subprogram within a REMOTE BLOCK  */
,8,16,216,124,50,142,9,94,71
/* %s1 statement is not FORTRAN 77 standard  */
,5,0,2,4,1,6
/* %s1 block is unfinished  */
,6,0,30,4,255,255,70
/* %s1 statement does not match with %s2 statement  */
,8,0,2,167,1,181,29,11,2
/* incomplete control structure found at %s1 statement  */
,8,255,223,75,33,102,74,0,2
/* %s1 statement is not allowed in %s2 definition  */
,8,0,2,4,1,19,3,11,53
/* no such field name found in structure %s1  */
,10,70,255,255,54,83,18,102,3,33,0
/* multiple definition of field name %s1  */
,7,255,18,53,5,83,18,0
/* structure %s1 has not been defined  */
,6,33,0,25,1,39,52
/* structure %s1 has already been defined  */
,6,33,0,25,59,39,52
/* structure %s1 must contain at least one field  */
,8,33,0,14,162,74,177,132,83
/* recursion detected in definition of structure %s1  */
,7,255,30,3,53,5,33,0
/* illegal use of structure %s1 containing union  */
,9,36,141,5,33,0,231,255,255,71
/* allocatable arrays cannot be fields within structures  */
,11,219,255,139,16,7,255,202,142,255,255,50
/* an integer conditional expression is not FORTRAN 77 standard  */
,8,15,69,255,169,67,4,1,6
/* %s1 statement must be used within %s2 definition  */
,8,0,2,14,7,89,142,11,53
};
static const char __FAR Msg8960[] = {
/* name can only appear in an EXTERNAL statement once  */
 9,18,96,62,80,3,15,210,2,88
/* character function %s1 may not be called since size was declared as (*)  */
,13,24,12,0,182,1,7,255,157,40,65,163,44,91
/* %s1 can only be used an an argument to a subroutine  */
,11,0,96,62,7,89,15,15,17,8,9,139
/* name cannot appear in both an INTRINSIC and EXTERNAL statement  */
,10,18,16,80,3,158,15,213,51,210,2
/* expecting a subroutine name  */
,4,10,9,139,18
/* %s1 statement not allowed in main program  */
,7,0,2,1,19,3,179,107
/* not an intrinsic FUNCTION name  */
,6,1,15,37,255,99,18
/* name can only appear in an INTRINSIC statement once  */
,9,18,96,62,80,3,15,213,2,88
/* subprogram recursion detected  */
,3,50,255,30
/* two main program units in the same file  */
,13,255,255,65,179,107,255,255,72,3,28,255,33,32
/* only one unnamed %s1 is allowed in an executable program  */
,12,62,132,255,255,74,0,4,19,3,15,244,107
/* function referenced as a subroutine  */
,6,12,255,31,44,9,139
};
static const char __FAR Msg9216[] = {
/* substringing of function or statement function return value is not FORTRAN 77 standard  */
 12,255,37,5,12,13,2,12,137,197,4,1,6
/* substringing valid only for character variables and array elements  */
,12,255,37,255,255,76,62,20,24,90,51,38,238
};
static const char __FAR Msg9472[] = {
/* statement number %i1 has already been defined in line %d2  */
 10,2,27,42,25,59,39,52,3,31,41
/* statement function definition appears after first executable statement  */
,8,2,12,53,73,153,248,244,2
/* %s1 statement must not be branched to but was in line %d2  */
,13,0,2,14,1,7,255,151,8,45,65,3,31,41
/* branch to statement %i1 in line %d2 not allowed  */
,9,60,8,2,42,3,31,41,1,19
/* specification statement must appear before %s1 is initialized  */
,8,49,2,14,80,117,0,4,127
/* statement %i1 was referenced as a FORMAT statement in line %d2  */
,12,2,42,65,255,31,44,9,92,2,3,31,41
/* IMPLICIT statement appears too late  */
,6,113,2,73,110,255,10
/* this statement will never be executed due to the preceding branch  */
,12,57,2,255,44,7,245,168,8,28,255,27,60
/* expecting statement number  */
,3,10,2,27
/* statement number %i1 was not used as a FORMAT statement in line %d2  */
,13,2,27,42,65,1,89,44,9,92,2,3,31,41
/* specification statement appears too late  */
,6,49,2,73,110,255,10
/* %s1 statement not allowed after %s2 statement  */
,7,0,2,1,19,153,11,2
/* statement number must be 99999 or less  */
,9,2,27,14,7,255,76,13,255,236
/* statement number cannot be zero  */
,5,2,27,16,7,111
/* this statement could branch to itself  */
,7,57,2,233,60,8,255,230
/* missing statement number %i1 - used in line %d2  */
,9,26,2,27,42,58,89,3,31,41
/* undecodeable statement or misspelled word %s1  */
,8,255,255,68,2,13,255,247,0
/* statement %i1 will never be executed due to the preceding branch  */
,12,2,42,255,44,7,245,168,8,28,255,27,60
/* expecting keyword or symbolic name  */
,6,10,255,232,13,140,18
/* number in %s1 statement is longer than 5 digits  */
,10,27,3,0,2,4,255,15,78,255,70
/* position of DATA statement is not FORTRAN 77 standard  */
,9,255,255,12,5,151,2,4,1,6
};
static const char __FAR Msg9728[] = {
/* variable %s1 in array declarator must be in COMMON or a dummy argument  */
 13,22,0,3,38,234,14,7,3,23,13,9,76,17
/* adjustable/assumed size array %s1 must be a dummy argument  */
,10,255,128,40,38,0,14,7,9,76,17
/* invalid subscript expression  */
,5,21,255,255,51,67
/* invalid number of subscripts  */
,6,21,27,5,255,255,53
/* using %s1 name incorrectly without list  */
,8,196,0,18,173,255,255,82,48
/* cannot substring array name %s1  */
,5,16,192,38,18,0
/* %s1 treated as an assumed size array  */
,9,0,255,255,64,44,15,115,40,38
/* assumed size array %s1 cannot be used as an i/o list item or a format/unit identifier  */
,18,115,40,38,0,16,7,89,44,15,171,48,175,13,9,255,209,255,2
/* limit of 65535 elements per dimension has been exceeded  */
,12,86,5,255,72,238,255,255,10,235,25,39,170
};
static const char __FAR Msg9984[] = {
/* unexpected number or name %s1  */
 5,193,27,13,18,0
/* bad sequence of operators  */
,7,255,146,189,5,255,255,5
/* invalid operator  */
,2,21,63
/* expecting end of statement after right parenthesis  */
,9,10,77,5,2,153,255,255,31,133
/* expecting an asterisk  */
,3,10,15,155
/* expecting colon  */
,2,10,227
/* expecting colon or end of statement  */
,6,10,227,13,77,5,2
/* missing comma  */
,2,26,120
/* expecting end of statement  */
,4,10,77,5,2
/* expecting integer variable  */
,3,10,69,22
/* expecting %s1 name  */
,3,10,0,18
/* expecting an integer  */
,3,10,15,69
/* expecting INTEGER, REAL, or DOUBLE PRECISION variable  */
,5,10,212,13,112,22
/* missing operator  */
,2,26,63
/* expecting a slash  */
,5,10,9,255,255,40
/* expecting %s1 expression  */
,3,10,0,67
/* expecting a constant expression  */
,4,10,9,35,67
/* expecting INTEGER, REAL, or DOUBLE PRECISION expression  */
,5,10,212,13,112,67
/* expecting INTEGER or CHARACTER constant  */
,6,10,255,102,13,150,35
/* unexpected operator  */
,2,193,63
/* no closing quote on literal string  */
,8,70,160,255,29,87,255,241,191
/* missing or invalid constant  */
,4,26,13,21,35
/* expecting character constant  */
,3,10,24,35
};
static const char __FAR Msg10240[] = {
/* length specification before array declarator is not FORTRAN 77 standard  */
 8,47,49,117,38,234,4,1,6
/* %i1 is an illegal length for %s2 type  */
,8,42,4,15,36,47,20,11,34
/* length specifier in %s1 statement is not FORTRAN 77 standard  */
,8,47,64,3,0,2,4,1,6
/* length specification not allowed with type %s1  */
,7,47,49,1,19,29,34,0
/* type of %s1 has already been established as %s2  */
,10,34,5,0,25,59,39,255,192,44,11
/* type of %s1 has not been declared  */
,7,34,5,0,25,1,39,163
/* %s1 of type %s2 is illegal in %s3 statement  */
,9,0,5,34,11,4,36,3,144,2
};
static const char __FAR Msg10752[] = {
/* illegal use of %s1 name %s2 in %s3 statement  */
 9,36,141,5,0,18,11,3,144,2
/* symbolic name %s1 is longer than 6 characters  */
,10,140,18,0,4,255,15,78,255,71,159
/* %s1 has already been defined as a %s2  */
,8,0,25,59,39,52,44,9,11
/* %s1 %s2 has not been defined  */
,6,0,11,25,1,39,52
/* %s1 is an unreferenced symbol  */
,7,0,4,15,255,42,255,38
/* %s1 already belongs to this NAMELIST group  */
,8,0,59,255,149,8,57,214,252
/* %s1 has been used but not defined  */
,7,0,25,39,89,45,1,52
/* dynamically allocating %s1 is not FORTRAN 77 standard  */
,6,255,184,0,4,1,6
/* %s1 in NAMELIST %s2 is illegal  */
,6,0,3,214,11,4,36
};
static const char __FAR Msg11008[] = {
/* Usage: %s1 {- or /<option>} <file-spec> {- or /<option>}  */
 13,255,119,0,255,45,13,201,255,77,255,45,13,201
,0
/* generate a listing file  */
,4,68,9,130,32
/* listing file to printer  */
,6,130,32,8,255,255,16
/* listing file to terminal  */
,6,130,32,8,255,255,59
/* listing file to disk  */
,5,130,32,8,255,181
/* generate an error file  */
,4,68,15,82,32
/* issue extension messages  */
,3,174,247,131
/* issue unreferenced warning  */
,5,174,255,42,255,43
/* issue warning messages  */
,4,174,255,43,131
/* INTEGER/LOGICAL size 2/1  */
,5,255,103,40,255,66
/* list INCLUDE files  */
,3,48,211,84
/* display diagnostic messages  */
,3,255,182,131
/* explicit typing required  */
,2,255,197
/* extend line length  */
,3,246,31,47
,0
/* /trace /bounds  */
,2,255,63
/* define macro  */
,2,255,173
/* relax FORMAT type checking  */
,5,255,32,92,34,119
/* relax wild branch checking  */
,7,255,32,255,255,80,60,119
,0
,0
,0
,0
,0
,0
,0
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
,5,255,255,55,226,62
/* generate default libraries  */
,4,68,164,255,239
/* floating-point calls  */
,3,101,255,159
/* emulate 80x87 instructions  */
,4,255,186,149,128
/* in-line 80x87 instructions  */
,3,126,149,128
/* 80287 instructions  */
,3,255,73,128
/* 80387 instructions  */
,3,255,74,128
/* /fp3 optimized for Pentium  */
,4,202,105,20,93
/* 80x87 reverse compatibility  */
,4,149,255,255,30
/* line # debugging information  */
,5,31,255,46,99,85
/* full debugging information  */
,4,255,214,99,85
/* base pointer optimizations  */
,3,255,147,56
/* no call-ret optimizations  */
,4,70,255,156,56
/* disable optimizations  */
,3,255,180,56
/* no stack frame optimizations  */
,5,70,109,255,211,56
/* statement functions in-line  */
,4,2,255,216,126
/* loop optimizations  */
,2,54,56
/* math optimizations  */
,3,255,243,56
/* numerical optimizations  */
,4,255,255,2,56
/* precision optimizations  */
,3,255,28,56
/* instruction scheduling  */
,2,255,227
/* space optimizations  */
,4,255,255,42,56
/* time optimizations  */
,3,255,40,56
/* /o[b,bp,i,k,l,m,r,t,do]  */
,2,255,62
,0
,0
/* stack checking  */
,2,109,119
/* set data threshold  */
,5,138,66,255,255,63
/* local variables on the stack  */
,6,255,14,90,87,28,109
/* pass character descriptors  */
,4,186,24,255,177
/* SAVE local variables  */
,4,217,255,14,90
/* name object file  */
,4,18,255,22,32
/* constants in code segment  */
,4,97,3,61,188
,0
,0
,0
,0
,0
,0
/* stack calling convention  */
,3,109,255,158
/* Easy OMF-386 object files  */
,5,255,92,255,22,84
/* default windowed application  */
,5,164,255,255,81,220
/* multithread application  */
,3,255,252,220
,0
/* dynamic link library  */
,5,122,255,240,255,11
,0
/* automatic stack growing  */
,4,156,109,255,219
/* flat memory model  */
,4,255,204,55,103
/* small memory model  */
,5,255,255,41,55,103
/* compact memory model  */
,4,255,165,55,103
/* medium memory model  */
,4,255,244,55,103
/* large memory model  */
,4,255,233,55,103
/* assume 80386 processor  */
,2,255,143
/* /3 optimized for 80486  */
,5,145,105,20,255,75
/* /3 optimized for Pentium  */
,4,145,105,20,93
/* save/restore segment regs  */
,7,255,255,33,188,255,255,24
/* FS not fixed  */
,4,255,98,1,249
/* GS not fixed  */
,4,255,100,1,249
,0
/* generate code for Windows  */
,5,68,61,20,255,122
/* structure definition  */
,2,33,53
/* group name  */
,2,252,18
/* statement function  */
,2,2,12
/* statement function definition  */
,3,2,12,53
/* simple variable  */
,4,255,255,39,22
/* variable in COMMON  */
,3,22,3,23
/* equivalenced variable  */
,2,169,22
/* subprogram argument  */
,2,50,17
/* statement function argument  */
,3,2,12,17
/* array  */
,1,38
/* allocated array  */
,3,255,131,38
/* variable or array  */
,3,22,13,38
/* intrinsic function  */
,2,37,12
/* external subprogram  */
,3,255,199,50
,0
/* assignment  */
,1,154
/* arithmetic if  */
,2,255,138
/* at end of statement,  */
,6,74,77,5,255,255,45
/* Defining subprogram: %s1; first ISN is %u2  */
,7,255,87,248,255,105,4,199
/* library member %s1,  */
,5,255,11,255,245,198
/* column %d1,  */
,3,161,255,48
/* in %s1,  */
,2,3,198
,0
,0
,0
/* %d1 statements, %i2 bytes, %d3 extensions, %d4 warnings, %d5 errors  */
,4,143,255,255,46
/* Code size (in bytes):  */
,6,255,81,40,200,255,155
/* Number of errors:  */
,4,114,5,255,191
/* Compile time (in seconds):  */
,8,255,83,255,40,200,255,255,35
/* Number of warnings:  */
,5,114,5,255,255,77
/* Number of statements compiled:  */
,6,114,5,255,36,255,168
/* Number of extensions:  */
,4,114,5,255,198
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
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
,3,255,55,72
/* *Diagnostic Options*  */
,3,255,54,72
/* *Debugging Options*  */
,3,255,53,72
/* *Miscellaneous Options*  */
,3,255,58,72
/* *Floating-Point Options*  */
,3,255,56,72
/* *Optimizations*  */
,2,255,59
/* *Memory Models*  */
,2,255,57
/* *CPU Targets*  */
,2,255,50
/* *Compile and Link Options*  */
,6,255,52,51,255,109,72
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0
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
,4,68,255,255,32
/* generate bounds checking  */
,4,68,255,150,119
/* *Character Set Options*  */
,3,255,51,72
/* Chinese character set  */
,4,255,80,24,138
/* Japanese character set  */
,4,255,106,24,138
/* Korean character set  */
,4,255,107,24,138
/* generate file dependencies  */
,4,68,32,255,175
/* *Application Type*  */
,2,255,49
/* messages in resource file  */
,6,131,3,255,255,29,32
/* devices are carriage control  */
,6,255,178,81,255,160,75
/* operate quietly  */
,3,255,255,4
,0
,0
,0
,0
,0
/* generate browse information  */
,4,68,255,152,85
/* extend float-pt. precision  */
,4,246,250,255,28
/* loop optimizations/unrolling  */
,4,54,255,255,7
/* align COMMON segments  */
,6,255,129,23,255,255,36
/* LF with FF  */
,5,255,108,29,255,96
/* WATCOM debugging information  */
,4,255,121,99,85
/* DWARF debugging information  */
,4,255,86,99,85
/* Codeview debugging info.  */
,5,255,82,99,255,224
,0
,0
/* DO-variables do not overflow  */
,4,255,85,1,185
/* path for INCLUDE files  */
,6,255,255,9,20,211,84
/* enable Pentium FDIV check  */
,6,255,187,93,255,95,226
/* branch prediction  */
,4,60,255,255,14
/* mangle COMMON segment names  */
,6,255,242,23,188,255,19
/* promote intrinsic arguments  */
,5,255,255,17,37,95
/* allow comma separator  */
,6,255,132,120,255,255,38
/* /fp3 optimized for Pentium Pro  */
,5,202,105,20,93,215
/* /3 optimized for Pentium Pro  */
,5,145,105,20,93,215
/* optimize at expense of compile-time  */
,8,255,23,74,255,196,5,255,167
/* move register saves into flow graph  */
,6,255,251,135,255,255,34
/* loop optimizations/assume loop invariant float-pt. variables are initialized  */
,11,54,255,255,6,54,255,229,250,90,81,127
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
/* count= 84 Group:offset = AR:12, BD: 0, BD: 1, CC: 3, CC: 5,... */
          ,9,'s','t','a','t','e','m','e','n','t'
/* count= 81 Group:offset = BD: 1, CC: 1, CC: 2, DA: 6, DO: 0,... */
          ,2,'i','n'
/* count= 70 Group:offset = BD: 0, BD: 1, CC: 0, CC: 1, CC: 2,... */
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
          ,3,'f','o','r'
/* count= 19 Group:offset = CO: 6, DA: 7, DA: 9, FM:10, HO: 1,... */
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
          ,6,'m','e','m','o','r','y'
/* count= 10 Group:offset = CP: 3, CP: 8, CP: 9, MO: 1, MO: 2,... */
          ,13,'o','p','t','i','m','i','z','a','t'
              ,'i','o','n','s'
/* count= 10 Group:offset = MS:47, MS:48, MS:49, MS:50, MS:52,... */
          ,4,'t','h','i','s'
/* count= 10 Group:offset = CV: 0, DO: 4, FM:10, IL: 1, LI:11,... */
          ,1,'-'
/* count=  9 Group:offset = CO: 4, CP: 3, DO: 0, MO: 2, SM: 0,... */
          ,7,'a','l','r','e','a','d','y'
/* count=  9 Group:offset = CM: 0, CO: 0, DO: 0, SA: 2, SP:24,... */
          ,6,'b','r','a','n','c','h'
/* count=  9 Group:offset = SP: 8, SP: 9, SP:12, ST: 3, ST: 7,... */
          ,4,'c','o','d','e'
/* count=  9 Group:offset = FM: 2, FM: 7, FM:10, FM:15, FM:20,... */
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
          ,8,'g','e','n','e','r','a','t','e'
/* count=  8 Group:offset = MS: 2, MS: 6, MS:37, MS:95, MS:192,... */
          ,7,'i','n','t','e','g','e','r'
/* count=  8 Group:offset = DA: 0, DA: 1, KO: 3, KO: 5, MD: 6,... */
          ,2,'n','o'
/* count=  8 Group:offset = CC: 2, FM:11, FM:12, IL: 6, SP:21,... */
          ,5,'B','L','O','C','K'
/* count=  7 Group:offset = BD: 1, CM: 4, SP: 4, SP:12, SP:13,... */
          ,8,'O','p','t','i','o','n','s','*'
/* count=  7 Group:offset = MS:161, MS:162, MS:163, MS:164, MS:165,... */
          ,7,'a','p','p','e','a','r','s'
/* count=  7 Group:offset = AR: 1, CM: 4, DA: 2, SA: 1, ST: 1,... */
          ,2,'a','t'
/* count=  7 Group:offset = DO: 1, EV: 1, SP: 7, SP:19, SP:25,... */
          ,7,'c','o','n','t','r','o','l'
/* count=  7 Group:offset = EY: 1, IL: 4, IL: 5, SP: 4, SP: 8,... */
          ,5,'d','u','m','m','y'
/* count=  7 Group:offset = AR: 1, AR:12, SF: 0, SF: 2, SF: 4,... */
          ,3,'e','n','d'
/* count=  7 Group:offset = FM: 9, SP:10, SX: 3, SX: 6, SX: 8,... */
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
          ,5,'f','i','l','e','s'
/* count=  6 Group:offset = IL: 7, IL:13, SM: 5, MS:11, MS:76,... */
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
          ,7,'P','e','n','t','i','u','m'
/* count=  5 Group:offset = MS:43, MS:90, MS:220, MS:225, MS:226 */
          ,6,'R','E','M','O','T','E'
/* count=  5 Group:offset = SP: 4, SP:12, SP:13, SP:14, SP:15 */
          ,9,'a','r','g','u','m','e','n','t','s'
/* count=  5 Group:offset = AR: 0, LI: 5, SF: 0, SF: 5, MS:223 */
          ,3,'c','a','n'
/* count=  5 Group:offset = IM: 1, MD: 4, SR: 0, SR: 2, SR: 7 */
          ,9,'c','o','n','s','t','a','n','t','s'
/* count=  5 Group:offset = CN: 0, CN: 2, DA: 7, DA: 9, MS:68 */
          ,12,'c','o','n','t','i','n','u','a','t'
              ,'i','o','n'
/* count=  5 Group:offset = CC: 2, CC: 4, CC: 5, CC: 8, PR: 3 */
          ,9,'d','e','b','u','g','g','i','n','g'
/* count=  5 Group:offset = MS:45, MS:46, MS:213, MS:214, MS:215 */
          ,7,'e','x','c','e','e','d','s'
/* count=  5 Group:offset = CP:10, CP:11, CP:12, MO: 0, SP:10 */
          ,14,'f','l','o','a','t','i','n','g','-'
              ,'p','o','i','n','t'
/* count=  5 Group:offset = CN: 1, KO: 0, KO: 1, KO: 2, MS:38 */
          ,5,'f','o','u','n','d'
/* count=  5 Group:offset = FM: 9, LI: 7, LI:13, SP:19, SP:21 */
          ,5,'m','o','d','e','l'
/* count=  5 Group:offset = MS:83, MS:84, MS:85, MS:86, MS:87 */
          ,4,'m','o','r','e'
/* count=  5 Group:offset = AR: 1, CC: 5, CM: 4, FM: 9, IL: 5 */
          ,9,'o','p','t','i','m','i','z','e','d'
/* count=  5 Group:offset = MS:43, MS:89, MS:90, MS:225, MS:226 */
          ,6,'o','p','t','i','o','n'
/* count=  5 Group:offset = CO: 1, CO: 2, CO: 3, CO: 4, CO: 5 */
          ,7,'p','r','o','g','r','a','m'
/* count=  5 Group:offset = KO: 4, RE: 1, SR: 5, SR: 9, SR:10 */
          ,5,'r','a','n','g','e'
/* count=  5 Group:offset = DO: 1, IM: 0, LI: 4, SP: 4, SP:10 */
          ,5,'s','t','a','c','k'
/* count=  5 Group:offset = MS:50, MS:62, MS:64, MS:75, MS:82 */
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
          ,8,'c','h','e','c','k','i','n','g'
/* count=  4 Group:offset = MS:18, MS:19, MS:62, MS:193 */
          ,5,'c','o','m','m','a'
/* count=  4 Group:offset = DO: 5, FM: 0, SX: 7, MS:224 */
          ,8,'c','o','m','p','i','l','e','r'
/* count=  4 Group:offset = CO: 6, CO: 7, CP: 4, MO: 0 */
          ,7,'d','y','n','a','m','i','c'
/* count=  4 Group:offset = CP: 8, CP: 9, MO: 2, MS:80 */
          ,4,'e','d','i','t'
/* count=  4 Group:offset = FM: 4, FM: 5, FM:12, FM:19 */
          ,4,'f','r','o','m'
/* count=  4 Group:offset = KO: 4, SP: 3, SP: 8, SP:15 */
          ,4,'h','a','v','e'
/* count=  4 Group:offset = IL: 2, IL: 3, IL:12, SP: 0 */
          ,7,'i','n','-','l','i','n','e'
/* count=  4 Group:offset = PR: 5, PR: 7, MS:40, MS:51 */
          ,11,'i','n','i','t','i','a','l','i','z'
              ,'e','d'
/* count=  4 Group:offset = BD: 0, DA: 3, ST: 4, MS:229 */
          ,12,'i','n','s','t','r','u','c','t','i'
              ,'o','n','s'
/* count=  4 Group:offset = MS:39, MS:40, MS:41, MS:42 */
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
          ,2,'/','3'
/* count=  3 Group:offset = MS:89, MS:90, MS:226 */
          ,1,'1'
/* count=  3 Group:offset = CC: 7, FM:19, PR: 3 */
          ,1,'2'
/* count=  3 Group:offset = EV: 0, EV: 1, LI: 5 */
          ,3,'3','2','K'
/* count=  3 Group:offset = CP:10, CP:11, CP:12 */
          ,5,'8','0','x','8','7'
/* count=  3 Group:offset = MS:39, MS:40, MS:44 */
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
          ,9,'a','u','t','o','m','a','t','i','c'
/* count=  3 Group:offset = CP:10, CP:12, MS:82 */
          ,5,'b','l','a','n','k'
/* count=  3 Group:offset = CC: 4, CM: 5, DA: 3 */
          ,4,'b','o','t','h'
/* count=  3 Group:offset = IL:12, SP: 0, SR: 3 */
          ,10,'c','h','a','r','a','c','t','e','r'
              ,'s'
/* count=  3 Group:offset = HO: 1, IM: 0, VA: 1 */
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
          ,7,'s','e','g','m','e','n','t'
/* count=  3 Group:offset = MS:68, MS:91, MS:222 */
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
          ,4,'/','f','p','3'
/* count=  2 Group:offset = MS:43, MS:225 */
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
          ,3,'P','r','o'
/* count=  2 Group:offset = MS:225, MS:226 */
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
/* count=  2 Group:offset = MS:77, MS:78 */
          ,9,'a','r','g','u','m','e','n','t',','
/* count=  2 Group:offset = LI: 7, LI:13 */
          ,9,'a','t','t','r','i','b','u','t','e'
/* count=  2 Group:offset = PR: 1, PR: 2 */
          ,6,'b','l','o','c','k','s'
/* count=  2 Group:offset = EY: 1, SP:11 */
          ,4,'b','y','t','e'
/* count=  2 Group:offset = PR: 5, PR: 6 */
          ,4,'c','a','s','e'
/* count=  2 Group:offset = SP:11 */
          ,5,'c','h','e','c','k'
/* count=  2 Group:offset = MS:36, MS:220 */
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
          ,5,'f','i','x','e','d'
/* count=  2 Group:offset = MS:92, MS:93 */
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
/* count=  2 Group:offset = MS:80, MS:115 */
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
          ,6,'o','b','j','e','c','t'
/* count=  2 Group:offset = MS:67, MS:76 */
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
          ,23,'/','o','[','b',',','b','p',',','i'
              ,',','k',',','l',',','m',',','r',',','t'
              ,',','d','o',']'
/* count=  1 Group:offset = MS:59 */
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
          ,5,'8','0','2','8','7'
/* count=  1 Group:offset = MS:41 */
          ,5,'8','0','3','8','7'
/* count=  1 Group:offset = MS:42 */
          ,5,'8','0','4','8','6'
/* count=  1 Group:offset = MS:89 */
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
          ,12,'E','a','s','y',' ','O','M','F','-'
              ,'3','8','6'
/* count=  1 Group:offset = MS:76 */
          ,6,'E','w','.','d','D','e'
/* count=  1 Group:offset = FM: 2 */
          ,6,'E','w','.','d','Q','e'
/* count=  1 Group:offset = FM:20 */
          ,4,'F','D','I','V'
/* count=  1 Group:offset = MS:220 */
          ,2,'F','F'
/* count=  1 Group:offset = MS:212 */
          ,4,'F','I','L','E'
/* count=  1 Group:offset = IL:12 */
          ,2,'F','S'
/* count=  1 Group:offset = MS:92 */
          ,8,'F','U','N','C','T','I','O','N'
/* count=  1 Group:offset = SR: 6 */
          ,2,'G','S'
/* count=  1 Group:offset = MS:93 */
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
          ,7,'W','i','n','d','o','w','s'
/* count=  1 Group:offset = MS:95 */
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
          ,22,'a','s','s','u','m','e',' ','8','0'
              ,'3','8','6',' ','p','r','o','c','e','s'
              ,'s','o','r'
/* count=  1 Group:offset = MS:88 */
          ,9,'a','t','t','e','m','p','t','e','d'
/* count=  1 Group:offset = SP:14 */
          ,9,'b','a','c','k','w','a','r','d','s'
/* count=  1 Group:offset = DO: 0 */
          ,3,'b','a','d'
/* count=  1 Group:offset = SX: 1 */
          ,12,'b','a','s','e',' ','p','o','i','n'
              ,'t','e','r'
/* count=  1 Group:offset = MS:47 */
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
          ,18,'c','a','l','l','i','n','g',' ','c'
              ,'o','n','v','e','n','t','i','o','n'
/* count=  1 Group:offset = MS:75 */
          ,5,'c','a','l','l','s'
/* count=  1 Group:offset = MS:38 */
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
          ,7,'c','o','m','p','a','c','t'
/* count=  1 Group:offset = MS:85 */
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
          ,7,'e','m','u','l','a','t','e'
/* count=  1 Group:offset = MS:39 */
          ,6,'e','n','a','b','l','e'
/* count=  1 Group:offset = MS:220 */
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
          ,4,'f','l','a','t'
/* count=  1 Group:offset = MS:83 */
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
          ,5,'f','r','a','m','e'
/* count=  1 Group:offset = MS:50 */
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
          ,7,'g','r','o','w','i','n','g'
/* count=  1 Group:offset = MS:82 */
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
          ,5,'l','a','r','g','e'
/* count=  1 Group:offset = MS:87 */
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
/* count=  1 Group:offset = MS:80 */
          ,7,'l','i','t','e','r','a','l'
/* count=  1 Group:offset = SX:20 */
          ,6,'m','a','n','g','l','e'
/* count=  1 Group:offset = MS:222 */
          ,4,'m','a','t','h'
/* count=  1 Group:offset = MS:53 */
          ,6,'m','e','d','i','u','m'
/* count=  1 Group:offset = MS:86 */
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
/* count=  1 Group:offset = MS:78 */
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
          ,4,'r','e','g','s'
/* count=  1 Group:offset = MS:91 */
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
          ,21,'r','e','v','e','r','s','e',' ','c'
              ,'o','m','p','a','t','i','b','i','l','i'
              ,'t','y'
/* count=  1 Group:offset = MS:44 */
          ,5,'r','i','g','h','t'
/* count=  1 Group:offset = SX: 3 */
          ,18,'r','u','n','-','t','i','m','e',' '
              ,'t','r','a','c','e','b','a','c','k'
/* count=  1 Group:offset = MS:192 */
          ,12,'s','a','v','e','/','r','e','s','t'
              ,'o','r','e'
/* count=  1 Group:offset = MS:91 */
          ,21,'s','a','v','e','s',' ','i','n','t'
              ,'o',' ','f','l','o','w',' ','g','r','a'
              ,'p','h'
/* count=  1 Group:offset = MS:228 */
          ,9,'s','e','c','o','n','d','s',')',':'
/* count=  1 Group:offset = MS:124 */
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
          ,5,'s','m','a','l','l'
/* count=  1 Group:offset = MS:84 */
          ,5,'s','p','a','c','e'
/* count=  1 Group:offset = MS:57 */
          ,9,'s','p','e','c','i','f','i','e','d'
/* count=  1 Group:offset = PR: 1 */
          ,7,'s','p','e','c','i','f','y'
/* count=  1 Group:offset = IL: 5 */
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
          ,6,'s','y','n','t','a','x'
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
/* Total number of phrases = 595 */
