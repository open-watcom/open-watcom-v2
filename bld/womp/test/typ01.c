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


typedef unsigned char   uint_8;
typedef unsigned short  uint_16;
typedef unsigned long   uint_32;
uint_8  v_uint_8;
uint_16 v_uint_16;
uint_32 v_uint_32;

typedef   signed char   int_8;
typedef   signed short  int_16;
typedef   signed long   int_32;
int_8   v_int_8;
int_16  v_int_16;
int_32  v_int_32;

typedef float           real_4;
typedef double          real_8;
real_4  v_real_4;
real_8  v_real_8;

typedef char            array_of_char_20[20];
typedef uint_8          array_of_uint_8_20[20];
array_of_char_20    v_array_of_char_20;
array_of_uint_8_20  v_array_of_uint_8_20;

typedef struct {
    uint_8      u8;
    uint_16     u16;
    uint_32     u32;
} unnamed_struct;
struct named_struct {
    uint_8      u8;
    uint_16     u16;
    uint_32     u32;
};
unnamed_struct      v_unnamed_struct;
struct named_struct v_named_struct;

typedef void near       *pointer_near;
typedef void far        *pointer_far;
typedef char huge       *pointer_huge;
pointer_near    v_pointer_near;
pointer_far     v_pointer_far;
pointer_huge    v_pointer_huge;

struct linked_list {
    struct linked_list  *next;
    char                *name;
};
struct linked_list  *v_struct_linked_list;

struct mutually_recursive_1 {
    struct mutually_recursive_2 *ref_2;
    uint_32                     a_big_number;
};
struct mutually_recursive_2 {
    struct mutually_recursive_1 *ref_1;
    uint_32                     another_big_number;
};
struct mutually_recursive_1 v_mutually_recursive_1;
struct mutually_recursive_2 v_mutually_recursive_2;

enum named_enum {
    RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET
};
typedef enum {
    MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
} unnamed_enum;
enum named_enum v_named_enum;
unnamed_enum v_unnamed_enum;

struct {
    uint_8  u8_1 : 1;
    uint_8  u8_2 : 2;
    int_8   i8_1 : 1;
    int_8   i8_2 : 2;
} v_bitfields;
