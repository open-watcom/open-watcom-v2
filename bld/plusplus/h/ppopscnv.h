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


// PPOPSCNV.H -- conversion numbers for operand-conversions of operators
//
// OPOVLOAD.C -- includes file several times to build tables
// PTREE.C    -- includes file several times to construct operator numbers.
//
// 92/01/29 -- J.W.Welch    -- defined
// 93/12/09 -- J.W.Welch    -- re-ordered and updated for new ambiguity
// 94/07/07 -- Greg Bentz   -- add OPM_NV
// 96/06/07 -- Lisa White   -- added OPM_PP to OPCNV_MINUS i.e. char* - char*
//                          -- added OPM_PI to EQAP i.e. char* += int
// 96/06/10 -- Lisa White   -- removed OPM_RP from EQAP
//          -- Lisa White   -- added OPM_NV to LOG2
// 96/06/11 -- Lisa White   -- removed 96/06/07 add of OPM_PI to EQAP, and
//                             instead added OPM_RPI (ptr &, int)
//                             added four new prototypes for OPM_RPI
//                             added OPM_RA | OPM_RPI | OPM_RP  to EQUALS
// 97/07/17 -- J.W.Welch    -- upgraded for 64 bitters

// The following define basic prototypes for dummy functions used in
// OPOVLOAD.C for conversion of operands of operators.
//
// The table must be ordered such that when only these operators are
// ambiguous, the last will be selected.
//
// Note: OPM_RA, OPM_RP, OPM_RI, OPM_ASSIGN require reference entries
//       others require value entries.
//       OPM_NV is used for standard operators where pointer scaling
//       would be required and, hence, where void* is not allowed.
//
#ifdef OPBASIC
//
// UNARY OPERATORS
//
{   OPBASIC( BOOL,       MAX,        OPM_B  | OPM_I  | OPM_A        )
,   OPBASIC( CHAR,       MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( SCHAR,      MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( UCHAR,      MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( WCHAR,      MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( SSHORT,     MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( USHORT,     MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( SINT,       MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( UINT,       MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( SLONG,      MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( ULONG,      MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( SLONG64,    MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( ULONG64,    MAX,        OPM_I  | OPM_A                 )
,   OPBASIC( FLOAT,      MAX,        OPM_A                          )
,   OPBASIC( DOUBLE,     MAX,        OPM_A                          )
,   OPBASIC( LONG_DOUBLE,MAX,        OPM_A                          )
,   OPBASIC( POINTER,    MAX,        OPM_P  | OPM_A                 )
//
// BINARY OPERATORS
//
,   OPBASIC( BOOL,       BOOL,       OPM_BB                         )
,   OPBASIC( CHAR,       CHAR,       OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SCHAR,      SCHAR,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( UCHAR,      UCHAR,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( WCHAR,      WCHAR,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SSHORT,     SSHORT,     OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( USHORT,     USHORT,     OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SINT,       SINT,       OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SINT,       SINT,       OPM_II | OPM_AA | OPM_SH       )
,   OPBASIC( SINT,       UINT,       OPM_SH                         )
,   OPBASIC( SINT,       SLONG,      OPM_SH                         )
,   OPBASIC( SINT,       ULONG,      OPM_SH                         )
,   OPBASIC( SINT,       SLONG64,    OPM_SH                         )
,   OPBASIC( SINT,       ULONG64,    OPM_SH                         )
,   OPBASIC( SINT,       POINTER,    OPM_IP                         )
,   OPBASIC( UINT,       SINT,       OPM_SH                         )
,   OPBASIC( UINT,       UINT,       OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( UINT,       UINT,       OPM_II | OPM_AA | OPM_SH       )
,   OPBASIC( UINT,       SLONG,      OPM_SH                         )
,   OPBASIC( UINT,       ULONG,      OPM_SH                         )
,   OPBASIC( UINT,       SLONG64,    OPM_SH                         )
,   OPBASIC( UINT,       ULONG64,    OPM_SH                         )
,   OPBASIC( UINT,       POINTER,    OPM_IP                         )
,   OPBASIC( SLONG,      SINT,       OPM_SH                         )
,   OPBASIC( SLONG,      UINT,       OPM_SH                         )
,   OPBASIC( SLONG,      SLONG,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SLONG,      SLONG,      OPM_II | OPM_AA | OPM_SH       )
,   OPBASIC( SLONG,      ULONG,      OPM_SH                         )
,   OPBASIC( SLONG,      SLONG64,    OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SLONG,      ULONG64,    OPM_SH                         )
,   OPBASIC( SLONG,      POINTER,    OPM_IP                         )
,   OPBASIC( ULONG,      SINT,       OPM_SH                         )
,   OPBASIC( ULONG,      UINT,       OPM_SH                         )
,   OPBASIC( ULONG,      SLONG,      OPM_SH                         )
,   OPBASIC( ULONG,      ULONG,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( ULONG,      ULONG,      OPM_II | OPM_AA | OPM_SH       )
,   OPBASIC( ULONG,      SLONG64,    OPM_SH                         )
,   OPBASIC( ULONG,      ULONG64,    OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( ULONG,      POINTER,    OPM_IP                         )
,   OPBASIC( SLONG64,    SINT,       OPM_SH                         )
,   OPBASIC( SLONG64,    UINT,       OPM_SH                         )
,   OPBASIC( SLONG64,    SLONG,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SLONG64,    ULONG,      OPM_SH                         )
,   OPBASIC( SLONG64,    SLONG64,    OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( SLONG64,    SLONG64,    OPM_II | OPM_AA | OPM_SH       )
,   OPBASIC( SLONG64,    ULONG64,    OPM_SH                         )
,   OPBASIC( SLONG64,    POINTER,    OPM_IP                         )
,   OPBASIC( ULONG64,    SINT,       OPM_SH                         )
,   OPBASIC( ULONG64,    UINT,       OPM_SH                         )
,   OPBASIC( ULONG64,    SLONG,      OPM_SH                         )
,   OPBASIC( ULONG64,    ULONG,      OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( ULONG64,    SLONG64,    OPM_SH                         )
,   OPBASIC( ULONG64,    ULONG64,    OPM_RA | OPM_RI | OPM_ASSIGN   )
,   OPBASIC( ULONG64,    ULONG64,    OPM_II | OPM_AA | OPM_SH       )
,   OPBASIC( ULONG64,    POINTER,    OPM_IP                         )
,   OPBASIC( FLOAT,      FLOAT,      OPM_RA          | OPM_ASSIGN   )
,   OPBASIC( FLOAT,      FLOAT,      OPM_AA                         )
,   OPBASIC( DOUBLE,     DOUBLE,     OPM_RA          | OPM_ASSIGN   )
,   OPBASIC( DOUBLE,     DOUBLE,     OPM_AA                         )
,   OPBASIC( LONG_DOUBLE,LONG_DOUBLE,OPM_RA          | OPM_ASSIGN   )
,   OPBASIC( LONG_DOUBLE,LONG_DOUBLE,OPM_AA                         )
,   OPBASIC( POINTER,    SINT,       OPM_PI                         )
,   OPBASIC( POINTER,    UINT,       OPM_PI                         )
,   OPBASIC( POINTER,    SLONG,      OPM_PI                         )
,   OPBASIC( POINTER,    ULONG,      OPM_PI                         )
,   OPBASIC( POINTER,    SLONG64,    OPM_PI                         )
,   OPBASIC( POINTER,    ULONG64,    OPM_PI                         )
,   OPBASIC( POINTER,    SINT,       OPM_RPI | OPM_ASSIGN           )
,   OPBASIC( POINTER,    UINT,       OPM_RPI | OPM_ASSIGN           )
,   OPBASIC( POINTER,    SLONG,      OPM_RPI | OPM_ASSIGN           )
,   OPBASIC( POINTER,    ULONG,      OPM_RPI | OPM_ASSIGN           )
,   OPBASIC( POINTER,    SLONG64,    OPM_RPI | OPM_ASSIGN           )
,   OPBASIC( POINTER,    ULONG64,    OPM_RPI | OPM_ASSIGN           )
,   OPBASIC( POINTER,    POINTER,    OPM_RP |          OPM_ASSIGN   )
,   OPBASIC( POINTER,    POINTER,    OPM_PP                         )
,   OPBASIC( POINTER, MEMBER_POINTER,OPM_PM                         )
,   OPBASIC( MEMBER_POINTER, MEMBER_POINTER, OPM_MM                 )

// the following two must be the last two; used for ":" overloading

,   OPBASIC( MAX,        MAX,        OPM_RR                         )
,   OPBASIC( MAX,        MAX,        0                              )
}
#endif

#ifdef OPBASIC_EXTRA
{   OPBASIC_EXTRA( POINTER, POINTER, OPM_PP | OPM_ASSIGN, GETOP_CONST     )
,   OPBASIC_EXTRA( POINTER, POINTER, OPM_PP | OPM_ASSIGN, GETOP_VOLATILE  )
,   OPBASIC_EXTRA( POINTER, POINTER, OPM_PP | OPM_ASSIGN, GETOP_CONST | GETOP_VOLATILE    )
}
#endif

// The following describe the basic prototypes used with each pattern.
//
#ifdef OPPRO
{   OPPRO( AA )                         // ( arith, arith )
,   OPPRO( BB )                         // ( boolean, boolean )
,   OPPRO( II )                         // ( integer, integer )
,   OPPRO( PP )                         // ( ptr, ptr )
,   OPPRO( A  )                         // ( arith )
,   OPPRO( RA )                         // ( ref.arith )
,   OPPRO( P  )                         // ( ptr )
,   OPPRO( RP )                         // ( ref.ptr )
,   OPPRO( I  )                         // ( integer )
,   OPPRO( RI )                         // ( ref.integer )
,   OPPRO( IP )                         // ( integer, ptr )
,   OPPRO( PI )                         // ( ptr, integer )
,   OPPRO( RPI )                        // ( ptr &, integer )
,   OPPRO( B  )                         // ( boolean )
,   OPPRO( MM )                         // ( ptr-memb, ptr-memb )
,   OPPRO( PM )                         // ( ptr, ptr-memb )
,   OPPRO( FUN )                        // function call
,   OPPRO( ASSIGN )                     // does assignment
,   OPPRO( QUEST )                      // ? operator
//  OPPRO( SCALAR_ASSIGN )              // indicates scalar assignment (set)
,   OPPRO( POST )                       // op++, op--
,   OPPRO( SUB )                        // subscript
,   OPPRO( SH )                         // shift operation
,   OPPRO( RR )                         // colon overloading
,   OPPRO( LT )                         // overload only left
,   OPPRO( NV )                         // don't allow void *
}
#endif

// The following defines the possible conversions for various classes
// of operators. The first column produces a number (OPCNV_...) which
// is used to index a vector of masks. A mask is used with each basic
// prototype used to choose the required dummy functions.
//
// The vector is generated in OPOVLOAD.C.
//
// The number is stored in the operator-flags table in PTREE.C.
//
#ifdef OPCNV
{   OPCNV( PLUS,      OPM_AA | OPM_PI | OPM_IP | OPM_NV ) // op + op
,   OPCNV( MINUS,     OPM_PP | OPM_AA | OPM_PI | OPM_NV ) // op - op
,   OPCNV( ONE_AR,    OPM_A                             ) // one arithmetic
,   OPCNV( ONE_RAP,   OPM_RA | OPM_RP | OPM_ASSIGN|OPM_NV)// one ref.(ar,ptr)
,   OPCNV( POST_OP,   OPM_RA | OPM_RP | OPM_POST | OPM_NV)// op++, op--
,   OPCNV( TWO_AR,    OPM_AA                            ) // two arithmetic
,   OPCNV( ONE_INT,   OPM_I                             ) // one integer
,   OPCNV( TWO_INT,   OPM_II                            ) // two integer
,   OPCNV( SHIFT,     OPM_SH                            ) // shift operation
,   OPCNV( RELEQ,     OPM_AA | OPM_PP | OPM_MM          ) // ==, !=
//  OPCNV( COLON,     OPM_AA | OPM_PP | OPM_MM | OPM_RR ) // : (in ?)
,   OPCNV( NOT_USED,  OPM_AA | OPM_PP | OPM_MM | OPM_RR ) // : (in ?)
,   OPCNV( RELN,      OPM_AA | OPM_PP                   ) // >=, <=, >, <
,   OPCNV( LOG1,      OPM_B                             ) // logical op
,   OPCNV( LOG2,      OPM_BB                            ) // op logical op
,   OPCNV( SUBSCR,    OPM_PI | OPM_IP | OPM_SUB | OPM_NV) // subscript
,   OPCNV( PTR,       OPM_P                             ) // one pointer
,   OPCNV( FUNCTION,  OPM_FUN | OPM_LT                  ) // function call
,   OPCNV( EQAP,      OPM_RPI | OPM_RA | OPM_ASSIGN     ) // op assign (ar,ptr)
,   OPCNV( EQAR,      OPM_RA | OPM_ASSIGN               ) // op assign arith
,   OPCNV( EQINT,     OPM_RI | OPM_ASSIGN               ) // op assign int
,   OPCNV( QUESTION,  OPM_A | OPM_P | OPM_QUEST | OPM_LT) // ?:
,   OPCNV( ARROW,     OPM_LT                            ) // -> operator
,   OPCNV( EQUALS,    OPM_ASSIGN | OPM_RA | OPM_RPI | OPM_RP    ) // = operator
,   OPCNV( ARROW_STAR,OPM_PM                            ) // ->* operator
}
#endif
