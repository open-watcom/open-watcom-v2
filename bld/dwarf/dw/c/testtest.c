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
* Description:  Test various DWARF writer library routines.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "watcom.h"
#include "dw.h"
#include "dwarf.h"
#include "testcli.h"

dw_handle       FundamentalTypes[ DW_FT_MAX ];
dw_handle       ConstantFundamentalTypes[ DW_FT_MAX ];

dw_handle       ConstCharStar;
dw_handle       EnumColours;


void TestMacInfo( void )
{
    dw_macro                    macro;

    DWMacFini( Client, DWMacDef( Client, 5, "NULL" ), "0" );

    macro = DWMacDef( Client, 33, "MAX" );
    DWMacParam( Client, macro, "__a" );
    DWMacParam( Client, macro, "__b" );
    DWMacFini( Client, macro, "((__a)>(__b)?(__a):(__b))" );

    DWMacStartFile( Client, 54, "foobar.h" );

    DWMacFini( Client, DWMacDef( Client, 2, "TRUE" ), "1" );

    DWMacEndFile( Client );

    DWMacUnDef( Client, 60, "NULL" );

    DWMacUse( Client, 70, "MAX" );
}


void TestTypedef( void )
{
    dw_handle   name_ptr;
    dw_handle   volatile_name;

    /* typedef signed char uint_8; */
    DWDeclPos( Client, 10, 1 );
    DWTypedef( Client, FundamentalTypes[ DW_FT_SIGNED_CHAR ], "uint_8", 0, 0 );

    /* typedef const char *name_ptr; */
    DWDeclPos( Client, 11, 1 );
    name_ptr = DWTypedef( Client, ConstCharStar, "name_ptr", 0, 0 );

    /* typedef name_ptr other_ptr */
    DWDeclPos( Client, 12, 1 );
    DWTypedef( Client, name_ptr, "other_ptr", 0, 0 );

    /* typedef volatile name_ptr icky_ptr */
    DWDeclPos( Client, 15, 1 );
    volatile_name = DWModifier( Client, name_ptr, DW_MOD_VOLATILE );
    DWTypedef( Client, volatile_name, "icky_ptr", 0, 0 );
}


void TestPointer( void )
{
    /* const char * far *p */
    DWDeclPos( Client, 36, 12 );
    DWPointer( Client, ConstCharStar, DW_PTR_TYPE_FAR16 );
    /* same thing but a reference */
    DWDeclPos( Client, 36, 29 );
    DWPointer( Client, ConstCharStar, DW_FLAG_REFERENCE | DW_PTR_TYPE_FAR32 );
}


void TestString( void )
{
    dw_loc_handle               string_length;
    dw_handle                   string_hdl;
    dw_loc_handle               a_loc;
    dw_loc_id                   id;

    /* type InputBuffer = String( 512 ); -- I forget exact Ada syntax */
    DWDeclPos( Client, 50, 1 );
    DWString( Client, NULL, 512, "InputBuffer", 0, 0 );

    /* CHARACTER*(*) A */
    /* we'll pretend the length of the string is at -4[ebp] */
    DWDeclPos( Client, 50, 8 );
    id = DWLocInit( Client );
    DWLocOp( Client, id, DW_LOC_fbreg, -4 );
    string_length = DWLocFini( Client, id );
    string_hdl = DWString( Client, string_length, sizeof( uint_32 ),
        NULL, 0, 0 );
    DWLocTrash( Client, string_length );
    id = DWLocInit( Client );
    DWLocOp( Client, id, DW_LOC_fbreg, -8 );
    a_loc = DWLocFini( Client, id );
    DWDeclPos( Client, 50, 16 );
    DWVariable( Client, string_hdl, a_loc, NULL, NULL, "A", 0, 0 );
    DWLocTrash( Client, a_loc );
}

void TestArray( void )
{
#if 0
    dw_dim_info         dim_info;
    dw_vardim_info      vdim_info;
    dw_handle           array_hdl;
    dw_loc_handle       buf_loc;
    dw_loc_handle       buf2_loc;
    dw_loc_id           id;

    /* char buf[ 80 ]; */
    DWDeclPos( Client, 55, 79 );
    array_hdl = DWBeginArray( Client, 1 );
    dim_info.index_type = FundamentalTypes[ DW_FT_UNSIGNED ];
    dim_info.lo_data = 0;
    dim_info.hi_data = 79;
    DWArrayDimension( Client, &dim_info );
    DWEndArray( Client );
    SymHandles[ 3 ] = 0x1234bul;
    id = DWLocInit( Client );
    DWLocStatic( Client, id, 3 );
    buf_loc = DWLocFini( Client, id );
    DWDeclPos( Client, 55, 80 );
    DWVariable( Client, array_hdl, buf_loc, NULL, NULL, "buf", 0, DW_FLAG_GLOBAL );
    DWLocTrash( Client, buf_loc );

    /* INTEGER*4 A(1:N) */
    DWDeclPos( Client, 55, 81 );
    array_hdl = DWBeginArray( Client, 1 );
    dim_info.index_type = FundamentalTypes[ DW_FT_SIGNED ];
    dim_info.lo_data = 1;
    id = DWLocInit( Client );   /* assume N is at -4[ebp] */
    DWLocOp( Client, id, DW_LOC_fbreg, -4 );
    buf2_loc = DWLocFini( Client, id );
    vdim_info.count_data = (dw_handle)buf2_loc;
    DWArrayDimension( Client, &dim_info );
    DWLocTrash( Client, buf2_loc );
    DWEndArray( Client );
#endif
}

void TestEnum( void )
{
    dw_uconst                       value;

    /*
        enum colours {
            RED,
            GREEN,
            BLUE
        };
    */
    DWDeclFile( Client, "A_New_File" );
    DWDeclPos( Client, 1, 5 );
    EnumColours = DWBeginEnumeration( Client, 1, "colours", 0, 0 );
    value = 2;
    DWAddConstant( Client, value, "BLUE" );
    value = 1;
    DWAddConstant( Client, value, "GREEN" );
    value = 0;
    DWAddConstant( Client, value, "RED" );
    DWEndEnumeration( Client );
}


void TestStruct1( void )
{
    dw_handle           struct_div_t;
    dw_loc_handle       field_loc;
    dw_handle           union_hdl;
    dw_handle           struct_foo;
    dw_handle           ptr_to_foo;
    dw_loc_id           id;

    /* struct div_t */
    struct_div_t = DWStruct( Client, DW_ST_STRUCT );
    DWBeginStruct( Client, struct_div_t, 8, "div_t", 0, 0 );
    id = DWLocInit( Client );
    field_loc = DWLocFini( Client, id );
    DWAddField( Client, FundamentalTypes[ DW_FT_SIGNED ],
        field_loc, "quot", 0 );
    DWLocTrash( Client, field_loc );
    id = DWLocInit( Client );
    DWLocOp( Client, id, DW_LOC_plus_uconst, 4 );
    field_loc = DWLocFini( Client, id );
    DWAddField( Client, FundamentalTypes[ DW_FT_SIGNED ],
        field_loc, "rem", 0 );
    DWLocTrash( Client, field_loc );
    DWEndStruct( Client );

    /*
        struct foo {
            struct foo *next;
            int         type;
            union {
                int     a;
                float   b;
            } x;
        };
    */
    struct_foo = DWStruct( Client, DW_ST_STRUCT );
    DWBeginStruct( Client, struct_foo, 12, "foo", 0, 0 );
    ptr_to_foo = DWPointer( Client, struct_foo, DW_PTR_TYPE_DEFAULT );
    id = DWLocInit( Client );
    field_loc = DWLocFini( Client, id );
    DWAddField( Client, ptr_to_foo, field_loc, "next", 0 );
    DWLocTrash( Client, field_loc );
    id = DWLocInit( Client );
    DWLocOp( Client, id, DW_LOC_plus_uconst, 4 );
    field_loc = DWLocFini( Client, id );
    DWAddField( Client, FundamentalTypes[ DW_FT_SIGNED ],
        field_loc, "type", 0 );
    DWLocTrash( Client, field_loc );
    union_hdl = DWStruct( Client, DW_ST_UNION );
    DWBeginStruct( Client, union_hdl, 4, "wombat", 0, 0 );
    DWAddField( Client, FundamentalTypes[ DW_FT_SIGNED ], NULL, "a", 0 );
    DWAddField( Client, FundamentalTypes[ DW_FT_FLOAT ], NULL, "b", 0 );
    DWEndStruct( Client );
    id = DWLocInit( Client );
    DWLocOp( Client, id, DW_LOC_plus_uconst, 8 );
    field_loc = DWLocFini( Client, id );
    DWAddField( Client, union_hdl, field_loc, "x", 0 );
    DWLocTrash( Client, field_loc );
    DWEndStruct( Client );
}


void TestStruct2( void )
{
    dw_loc_handle               field_loc;
    dw_loc_id                   id;

    /*
        struct date {
            unsigned short      day     : 5;
            unsigned short      month   : 4;
            unsigned short      year    : 7;
        };
    */
    DWBeginStruct( Client, DWStruct( Client, DW_ST_STRUCT ), 2,
        "date", 0, 0 );
    id = DWLocInit( Client );
    field_loc = DWLocFini( Client, id );
    DWAddBitField( Client, FundamentalTypes[ DW_FT_UNSIGNED ],
        field_loc, 0, 11, 5, "day", 0 );
    DWAddBitField( Client, FundamentalTypes[ DW_FT_UNSIGNED ],
        field_loc, 0, 7, 4, "month", 0 );
    DWAddBitField( Client, FundamentalTypes[ DW_FT_UNSIGNED ],
        field_loc, 0, 0, 7, "year", 0 );
    DWLocTrash( Client, field_loc );
    DWEndStruct( Client );
}


void TestStruct3( void )
{
    dw_handle           class_1;
    dw_handle           class_2;
    dw_handle           class_3;
    dw_loc_handle       field_hdl;
    dw_loc_id           id;

    class_1 = DWStruct( Client, DW_ST_CLASS );
    class_2 = DWStruct( Client, DW_ST_CLASS );
    DWBeginStruct( Client, class_1, 4, "bugs", 0, 0 );
    DWAddFriend( Client, class_2 );
    /* static int a; */
    DWAddField( Client, FundamentalTypes[ DW_FT_SIGNED ], NULL, "a",
        DW_FLAG_PRIVATE );  // was DW_FLAG_STATIC ??

    /* private float b; */
    field_hdl = DWLocFini( Client, DWLocInit( Client ) );
    DWAddField( Client, FundamentalTypes[ DW_FT_FLOAT ], field_hdl, "b",
        DW_FLAG_PRIVATE );
    DWLocTrash( Client, field_hdl );
    DWEndStruct( Client );

    id = DWLocInit( Client );
    SymHandles[ 5 ] = 0x666ul;
    DWLocStatic( Client, id, 5 );
    field_hdl = DWLocFini( Client, id );
    DWVariable( Client, FundamentalTypes[ DW_FT_SIGNED ], field_hdl,
        class_1, NULL, "a", 0, 0 );
    DWLocTrash( Client, field_hdl );

    DWBeginStruct( Client, class_2, 4, "beetles", 0, 0 );
    field_hdl = DWLocFini( Client, DWLocInit( Client ) );
    DWAddField( Client, FundamentalTypes[ DW_FT_FLOAT ], field_hdl, "b",
        DW_FLAG_PRIVATE );
    DWLocTrash( Client, field_hdl );
    DWEndStruct( Client );

    class_3 = DWStruct( Client, DW_ST_CLASS );
    DWBeginStruct( Client, class_3, 16, "ants", 0, 0 );
    field_hdl = DWLocFini( Client, DWLocInit( Client ) );
    DWAddInheritance( Client, class_2, field_hdl, 0 );
    DWLocTrash( Client, field_hdl );
    DWEndStruct( Client );
}


void TestSubroutineType( void )
{
    dw_handle                   sub_type;

    sub_type = DWBeginSubroutineType( Client,
        FundamentalTypes[ DW_FT_SIGNED ], NULL, 0, DW_FLAG_PROTOTYPED );
    DWFormalParameter( Client, FundamentalTypes[ DW_FT_UNSIGNED_CHAR ], NULL,
        NULL, "a", DW_DEFAULT_NONE );
    DWEllipsis( Client );
    DWEndSubroutineType( Client );
}


void TestLexicalBlock( void )
{
    DWBeginLexicalBlock( Client, NULL, "FOOBAR" );
    TestTypedef();
    DWEndLexicalBlock( Client );
}


void TestCommonBlock( void )
{
    dw_handle                   common_block;
    dw_loc_id                   id;
    dw_loc_handle               loc;

    id = DWLocInit( Client );
    DWLocStatic( Client, id, 5 );
    loc = DWLocFini( Client, id );
    common_block = DWBeginCommonBlock( Client, loc, NULL, "DATA", 0 );
    DWLocTrash( Client, loc );
    id = DWLocInit( Client );
    DWLocOp( Client, id, DW_LOC_plus_uconst, 4 );
    loc = DWLocFini( Client, id );
    DWVariable( Client, FundamentalTypes[ DW_FT_UNSIGNED_CHAR ], loc, NULL,
        NULL, "UNCLE", 0, 0 );
    DWLocTrash( Client, loc );
    DWEndCommonBlock( Client );

    DWIncludeCommonBlock( Client, common_block );
}


void TestSubroutine( void )
{
    dw_handle                   inline_sub;

    inline_sub = DWBeginSubroutine( Client, DW_SB_NEAR_CALL,
        ConstCharStar, NULL, NULL, NULL, NULL, NULL, "inline_me_baby", 0,
        DW_FLAG_DECLARATION | DW_FLAG_PROTOTYPED | DW_FLAG_DECLARED_INLINE );
    TestTypedef();
    DWEndSubroutine( Client );

    DWBeginInlineSubroutine( Client, inline_sub, NULL, NULL );
    TestArray();
    DWEndSubroutine( Client );
}


void TestLine( void )
{
    /* try varying the column */
    DWLineNum( Client, DW_LN_STMT, 1, 1, 0 );
    DWLineNum( Client, DW_LN_DEFAULT, 1, 5, 6 );
    DWLineNum( Client, DW_LN_DEFAULT, 1, 30, 24 );

    /* let's start varying the line number */
    DWLineNum( Client, DW_LN_STMT, 2, 1, 40 );
    DWLineNum( Client, DW_LN_DEFAULT, 3, 1, 50 );
    DWLineNum( Client, DW_LN_DEFAULT, 2, 1, 60 );

    DWLineNum( Client, DW_LN_STMT, 5, 1, 70 );
    DWLineNum( Client, DW_LN_STMT, 10, 1, 80 );
    DWLineNum( Client, DW_LN_STMT, 11, 1, 180 );
    DWLineNum( Client, DW_LN_STMT, 12, 1, 480 );
    DWLineNum( Client, DW_LN_STMT, 25, 1, 480 );
    DWLineNum( Client, DW_LN_STMT, 35, 1, 1480 );
}


void TestAranges( void )
{
    RelocValues[ DW_W_ARANGE_ADDR ] = 0x5555;
    DWAddress( Client, 511 );
}


void TestPubnames( void )
{
    DWPubname( Client, ConstCharStar, "foobar" );
}


void Test( void )
{
    int                         i;
    char                        buf[10];

    /* create all the fundamental types */
    for( i = DW_FT_MIN; i < DW_FT_MAX; ++i ) {
        FundamentalTypes[ i ] = DWFundamental( Client, itoa(i,buf,10), i, 2 );
    }
    /* and let's get some constant versions of them */
    for( i = DW_FT_MIN; i < DW_FT_MAX; ++i ) {
        ConstantFundamentalTypes[ i ] =
            DWModifier( Client, FundamentalTypes[ i ], DW_MOD_CONSTANT );
    }
    ConstCharStar = DWPointer( Client,
        ConstantFundamentalTypes[ DW_FT_SIGNED_CHAR ], 0 );

    TestMacInfo();

    TestTypedef();
    TestPointer();
    TestString();
    TestArray();
    TestEnum();
    TestStruct1();
    TestStruct2();
    TestStruct3();
    TestSubroutineType();

    TestLexicalBlock();
    TestCommonBlock();

    TestSubroutine();

    TestLine();

    TestAranges();
    TestPubnames();
}
