//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
//
// Created: MBN 04/04/89 -- Initial design and implementation
// Updated: DKM 07/07/89 -- To work around Xenix 31 char limit:
//                          Shortened is_less_than        to is_lt
//                                    is_greater_than     to is_gt
//                                    is_less_or_equal    to is_le
//                                    is_greater_or_equal to is_ge
// Updated: MBN 12/15/89 -- Made case-flag optional on is_equal, is_not_equal
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/11/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/11/92 -- removed "fast" TOLOWER/UPPER; just use ANSI C's
//
// This   header  file   defines function   prototypes   for extended    char*
// functionality patterned after  that of the  CoolString class. This will allow a
// programmer to chose char* or CoolString strings  for an application  based upon
// the needs of the program and not the availability  of a particular function
// or functions. Note  that there  is no attempt  to  allocate  or  deallocate
// memory.  Functions for  comparison (both case  sensitive  and insensitive),
// case conversion and  string  token  trimming are provided.   All  functions
// assume pointer operands are passed.
//

#ifndef CHARH                   // If not yet defined,
#define CHARH                   // Indicate that char* header defined

#include <string.h>             // Include standard string header file
#include <iostream.h>

#ifndef MISCELANEOUSH                           // If no misc.h file
#include <cool/misc.h>                  // Include useful defintions
#endif  

void reverse (char*);           // Reverse character order in string

extern Boolean is_equal_n (const char*, const char*, int n,
                           Boolean case_flag=INSENSITIVE);

extern Boolean is_equal (const char*, const char*,
                         Boolean case_flag=INSENSITIVE);

extern Boolean is_not_equal (const char*, const char*,
                             Boolean case_flag=INSENSITIVE);

extern Boolean is_lt (const char*, const char*, Boolean);
extern Boolean is_gt (const char*, const char*, Boolean);
extern Boolean is_le (const char*, const char*, Boolean);
extern Boolean is_ge (const char*, const char*, Boolean);

extern char* c_trim (char*, const char*);       // Trim characters from string
extern char* c_left_trim (char*, const char*);  // Trim prefix characters
extern char* c_right_trim (char*, const char*); // Trim suffix characters

extern char* c_upcase (char*);                  // Convert string to upper case
extern char* c_downcase (char*);                // Convert string to lower case
extern char* c_capitalize (char*);              // Capitalize each word 
extern const char* strfind (const char*, const char*,
                            long* start=0, long* end=0);
extern const char* strrfind (const char*, const char*,
                             long* start=0, long* end=0);
extern char* strnremove (char*, long);
extern char* strndup (const char*, long);
extern char* strnyank (char*, long);

#define TO_LOWER tolower
#define TO_UPPER toupper

#endif                                          // End #ifdef of CHARH

