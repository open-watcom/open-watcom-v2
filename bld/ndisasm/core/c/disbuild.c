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
* Description:  Build tables that drive the disassembler.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "distypes.h"
#include "dis.h"

#define NUM_ELTS( a ) (sizeof( a ) / sizeof( a[0] ))

#define LENGTH_BIT      0x80

typedef struct {
    char                *string;
    unsigned            string_idx;
} string_data;

typedef struct {
    unsigned_32         opcode;
    unsigned_32         mask;
    unsigned            idx;
    char                *idx_name;
    char                *handler;
} ins_decode_data;

#if DISCPU & DISCPU_axp

ins_decode_data AXPDecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_AXP_##idx, #idx, #handler },
    #include "insaxp.h"
};

string_data AXPInsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insaxp.h"
};

string_data AXPRegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regaxp.h"
};

string_data AXPRefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refaxp.h"
};

unsigned AXPInsNum[] = {
    NUM_ELTS( AXPDecodeTable1 ),
    0
};

ins_decode_data *AXPDecodeTable[] = {
    AXPDecodeTable1,
    NULL
};

string_data *AXPInsTable[] = {
    AXPInsTable1,
    NULL
};

#endif

#if DISCPU & DISCPU_ppc

ins_decode_data PPCDecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_PPC_##idx, #idx, #handler },
    #include "insppc.h"
};

string_data PPCInsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insppc.h"
};

string_data PPCRegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regppc.h"
};

string_data PPCRefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refppc.h"
};

unsigned PPCInsNum[] = {
    NUM_ELTS( PPCDecodeTable1 ),
    0
};

ins_decode_data *PPCDecodeTable[] = {
    PPCDecodeTable1,
    NULL
};

string_data *PPCInsTable[] = {
    PPCInsTable1,
    NULL
};

#endif

#if DISCPU & DISCPU_x86

ins_decode_data X86DecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_X86_##idx, #idx, #handler },
    #include "insx86.h"
};

string_data X86InsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insx86.h"
};

ins_decode_data X86DecodeTable2[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_X86_##idx, #idx, #handler },
    #include "insx86e1.h"
};

string_data X86InsTable2[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insx86e1.h"
};

ins_decode_data X86DecodeTable3[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_X86_##idx, #idx, #handler },
    #include "insx86e2.h"
};

string_data X86InsTable3[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insx86e2.h"
};

ins_decode_data X86DecodeTable4[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_X86_##idx, #idx, #handler },
    #include "insx86e3.h"
};

string_data X86InsTable4[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insx86e3.h"
};

ins_decode_data X86DecodeTable5[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_X86_##idx, #idx, #handler },
    #include "insx86e4.h"
};

string_data X86InsTable5[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insx86e4.h"
};

string_data X86RegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regx86.h"
};

string_data X86RefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refx86.h"
};

unsigned X86InsNum[] = {
    NUM_ELTS( X86DecodeTable1 ),
    NUM_ELTS( X86DecodeTable2 ),
    NUM_ELTS( X86DecodeTable3 ),
    NUM_ELTS( X86DecodeTable4 ),
    NUM_ELTS( X86DecodeTable5 ),
    0
};

ins_decode_data *X86DecodeTable[] = {
    X86DecodeTable1,
    X86DecodeTable2,
    X86DecodeTable3,
    X86DecodeTable4,
    X86DecodeTable5,
    NULL
};

string_data *X86InsTable[] = {
    X86InsTable1,
    X86InsTable2,
    X86InsTable3,
    X86InsTable4,
    X86InsTable5,
    NULL
};

#endif

#if DISCPU & DISCPU_x64

ins_decode_data X64DecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_X64_##idx, #idx, #handler },
    #include "insx64.h"
};

string_data X64InsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insx64.h"
};

string_data X64RegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regx64.h"
};

string_data X64RefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refx64.h"
};

unsigned X64InsNum[] = {
    NUM_ELTS( X64DecodeTable1 ),
    0
};

ins_decode_data *X64DecodeTable[] = {
    X64DecodeTable1,
    NULL
};

string_data *X64InsTable[] = {
    X64InsTable1,
    NULL
};

#endif

#if DISCPU & DISCPU_jvm

ins_decode_data JVMDecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_JVM_##idx, #idx, #handler },
    #include "insjvm.h"
};

string_data JVMInsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insjvm.h"
};

string_data JVMRegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regjvm.h"
};

string_data JVMRefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refjvm.h"
};

unsigned JVMInsNum[] = {
    NUM_ELTS( JVMDecodeTable1 ),
    0
};

ins_decode_data *JVMDecodeTable[] = {
    JVMDecodeTable1,
    NULL
};

string_data *JVMInsTable[] = {
    JVMInsTable1,
    NULL
};

#endif

#if DISCPU & DISCPU_sparc

ins_decode_data SPARCDecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_SPARC_##idx, #idx, #handler },
    #include "inssparc.h"
};

string_data SPARCInsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "inssparc.h"
};

string_data SPARCRegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regsparc.h"
};

string_data SPARCRefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refsparc.h"
};

unsigned SPARCInsNum[] = {
    NUM_ELTS( SPARCDecodeTable1 ),
    0
};

ins_decode_data *SPARCDecodeTable[] = {
    SPARCDecodeTable1,
    NULL
};

string_data *SPARCInsTable[] = {
    SPARCInsTable1,
    NULL
};

#endif

#if DISCPU & DISCPU_mips

ins_decode_data MIPSDecodeTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { opcode, mask, DI_MIPS_##idx, #idx, #handler },
    #include "insmips.h"
};

string_data MIPSInsTable1[] = {
    #undef inspick
    #define inspick( idx, name, opcode, mask, handler ) { name, 0 },
    #include "insmips.h"
};

string_data MIPSRegTable[] = {
    #undef regpick
    #define regpick( idx, name ) { name, 0 },
    #include "regmips.h"
};

string_data MIPSRefTable[] = {
    #undef refpick
    #define refpick( idx, name ) { name, 0 },
    #include "refmips.h"
};

unsigned MIPSInsNum[] = {
    NUM_ELTS( MIPSDecodeTable1 ),
    0
};

ins_decode_data *MIPSDecodeTable[] = {
    MIPSDecodeTable1,
    NULL
};

string_data *MIPSInsTable[] = {
    MIPSInsTable1,
    NULL
};

#endif

typedef struct {
    string_data         *reg_names;
    unsigned            num_reg_names;
    string_data         *ref_names;
    unsigned            num_ref_names;
    unsigned            *num_ins;
    string_data         **ins_names;
    ins_decode_data     **decode;
    char                *prefix;
} machine_data;

#define TABLE( who )                    \
        {                               \
        who##RegTable,                  \
        NUM_ELTS( who##RegTable ),      \
        who##RefTable,                  \
        NUM_ELTS( who##RefTable ),      \
        who##InsNum,                    \
        who##InsTable,                  \
        who##DecodeTable,               \
        #who                            \
        }

machine_data AMachine[] = {
#if DISCPU & DISCPU_axp
    TABLE( AXP ),
#endif
#if DISCPU & DISCPU_ppc
    TABLE( PPC ),
#endif
#if DISCPU & DISCPU_x86
    TABLE( X86 ),
#endif
#if DISCPU & DISCPU_x64
    TABLE( X64 ),
#endif
#if DISCPU & DISCPU_jvm
    TABLE( JVM ),
#endif
#if DISCPU & DISCPU_sparc
    TABLE( SPARC ),
#endif
#if DISCPU & DISCPU_mips
    TABLE( MIPS ),
#endif
};

typedef struct string_list string_list;

struct string_list {
    string_list         *next;
    string_data         *data;
    unsigned            len;
};

#define MAX_ENTRIES     96*1024
string_list     *Strings;
unsigned char   StringTable[MAX_ENTRIES];
unsigned        StringIndex;
dis_selector    SelTable[MAX_ENTRIES];
unsigned        SelIndex;

typedef struct range    range;
struct range {
    range       *next;
    unsigned_32 check;
    unsigned    idx;
    unsigned    num;
    unsigned    entry[1]; /* variable sized */
};

static unsigned AddString( string_data *data )
{
    unsigned    len;
    string_list **owner;
    string_list *curr;
    string_list *new;

    len = strlen( data->string );
    owner = &Strings;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        if( curr->len < len ) break;
        owner = &curr->next;
    }
    new = malloc( sizeof( *new ) );
    if( new == NULL ) {
        fprintf( stderr, "Out of memory!\n" );
        exit( 1 );
    }
    *owner = new;
    new->next = curr;
    new->data = data;
    new->len = len;
    return( len );
}

static int StringMatch( unsigned idx, string_list *curr )
{
    unsigned            i;
    unsigned char       c;
    char                *str;

    str = curr->data->string;
    i = curr->len;
    for( ;; ) {
        if( i == 0 ) return( 1 );
        if( idx >= StringIndex ) return( 0 );
        c = StringTable[idx++];
        if( !(c & LENGTH_BIT) ) {
            if( c != *str ) return( 0 );
            ++str;
            --i;
        }
    }
}

static void BuildStringTable( void )
{
    string_list *curr;
    string_list *fix;
    string_list *next;
    unsigned    idx;

    for( curr = Strings; curr != NULL; curr = curr->next ) {
        idx = 0;
        for( ;; ) {
            if( idx >= StringIndex ) {
                curr->data->string_idx = StringIndex;
                StringTable[StringIndex++] = LENGTH_BIT | curr->len;
                memcpy( &StringTable[StringIndex], curr->data->string,
                                curr->len );
                StringIndex += curr->len;
                break;
            }
            if( StringMatch( idx, curr ) ) {
                if( (StringTable[idx]&~LENGTH_BIT) != curr->len ) {
                    memmove( &StringTable[idx+1], &StringTable[idx],
                            StringIndex - idx );
                    ++StringIndex;
                    StringTable[idx] = LENGTH_BIT | curr->len;
                    for( fix = Strings; fix != curr; fix = fix->next ) {
                        if( fix->data->string_idx >= idx ) {
                            fix->data->string_idx++;
                        }
                    }
                }
                curr->data->string_idx = idx;
                break;
            }
            ++idx;
        }
    }
    for( curr = Strings; curr != NULL; curr = next ) {
        next = curr->next;
        free( curr );
    }
}


static int BuildRanges( FILE *fp, ins_decode_data **_data, unsigned *_num,
                char *prefix )
{
    unsigned            i;
    unsigned            j;
    unsigned            idx;
    unsigned            bit_count[32];
    unsigned long       opcode_or;
    unsigned long       bits;
    unsigned long       curr_mask;
    unsigned            idx_mask;
    unsigned            shift;
    unsigned            best_bit;
    unsigned            shifted_mask;
    range               *head;
    range               *tail;
    range               *new;
    unsigned            first_sel;
    int                 dumped_entries;

    unsigned            num = *_num;
    ins_decode_data     *data = *_data;

    dumped_entries = 0;
    first_sel = SelIndex;
    head = malloc( sizeof( *head ) + num * sizeof( head->entry[0] ) );
    if( head == NULL ) {
        fprintf( stderr, "out of memory!\n" );
        exit( 1 );
    }
    tail = head;
    idx = 0;
    for( i = 0; i < num; ++i ) {
        /* Mask of zero is an instruction synonym */
        if( data[i].mask != 0 ) {
            head->entry[idx++] = i;
        }
    }
    head->num = idx;
    head->idx = 0;
    head->next = NULL;
    head->check = ~0;
    for( ;; ) {
        memset( bit_count, 0, sizeof( bit_count ) );
        opcode_or = 0;
        for( i = 0; i < head->num; ++i ) {
            opcode_or |= data[head->entry[i]].opcode & head->check;
        }
        for( i = 0; i < head->num; ++i ) {
            bits = data[head->entry[i]].mask & opcode_or;
            for( j = 0; j < 32; ++j ) {
                if( bits & (1UL << j) ) {
                    bit_count[j]++;
                }
            }
        }
        best_bit = 0;
        for( i = 0; i < 32; ++i ) {
            /* find the bit position that occurs in the most masks */
            if( bit_count[i] > bit_count[best_bit] ) best_bit = i;
        }
        shift = best_bit;
        if( bit_count[shift] != 0 ) {
            shifted_mask = 0;
            i = shift;
            for( ;; ) {
                if( i >= 32 ) break;
                /*
                    Don't include in range if less than 3/4ths of the
                    masks use the bit.
                */
                if( (head->check & (1UL << i))
                 && (bit_count[i] < (3*bit_count[best_bit]) / 4) ) break;
                shifted_mask <<= 1;
                shifted_mask |= 1;
                if( shifted_mask == 0xff ) break;
                ++i;
            }
            // now try to grow it the other way
            if( shift != 0 && shifted_mask != 0xff ) {
                i = shift - 1;
                for( ;; ) {
                    if( i == 0 ) break;
                    if( (head->check & (1UL << i))
                        && (bit_count[i] < (3*bit_count[best_bit]) / 4) ) break;
                    shifted_mask <<= 1;
                    shifted_mask |= 1;
                    --shift;
                    if( shifted_mask == 0xff ) break;
                    --i;
                }
            }
            dumped_entries++;
            fprintf( fp, "    { 0x%2.2x, 0x%2.2x, 0x%4.4x },\n", shifted_mask, shift, SelIndex );
            curr_mask = (unsigned long) shifted_mask << shift;
            for( i = 0; i <= shifted_mask; ++i ) {
                for( j = 0; j < head->num; ++j ) {
                    idx = (data[head->entry[j]].opcode >> shift) & shifted_mask;
                    idx_mask = (data[head->entry[j]].mask >> shift) & shifted_mask;
                    if( (i & idx_mask) == idx ) {
                        if( SelTable[SelIndex + i] == 0 ) {
                            SelTable[SelIndex + i] = head->entry[ j ] + 1;
                        } else {
                            if( SelTable[SelIndex + i] > 0 ) {
                                new = malloc( sizeof( *new ) + num * sizeof( new->entry[0] ) );
                                if( new == NULL ) {
                                    fprintf( stderr, "out of memory!\n" );
                                    exit( 1 );
                                }
                                new->next = NULL;
                                new->check = head->check &= ~curr_mask;
                                new->idx = tail->idx + 1;
                                new->num = 1;
                                new->entry[0] = SelTable[SelIndex + i] - 1;
                                SelTable[SelIndex + i] = -new->idx;
                                tail->next = new;
                                tail = new;
                            }
                            tail->entry[tail->num++] = head->entry[ j ];
                        }
                    }
                }
            }
            SelIndex += shifted_mask + 1;
            if( SelIndex >= MAX_ENTRIES ) {
                fprintf( stderr, "internal error (MAX_ENTRIES may need increasing)\n" );
                exit( 1 );
            }
        } else {
            /* multiple opcode/mask pairs going to same bit pattern */
            fprintf( stderr, "internal error constructing range table\n" );
            for( i = 0; i < head->num; ++i ) {
                fprintf( stderr, "    machine:%s, opcode:%8.8lx, mask:%8.8lx, idx:%s\n",
                        prefix, (unsigned long)data[head->entry[i]].opcode,
                                (unsigned long)data[head->entry[i]].mask,
                                data[head->entry[i]].idx_name );
            }
            exit( 1 );
        }
        new = head->next;
        free( head );
        head = new;
        if( head == NULL ) break;
    }
    if( !dumped_entries ) {
        dumped_entries++;
        fprintf( fp, "    { 0x00, 0x00, 0x0000 },\n" );
    }
    for( ; first_sel < SelIndex; ++first_sel ) {
        if( SelTable[first_sel] > 0 ) {
            SelTable[first_sel] = data[ SelTable[first_sel] - 1 ].idx;
        }
    }
    return( dumped_entries );
}

#define INVALID_INS     "????"
string_data InvalidIns = { INVALID_INS, 0 };
string_data NullString = { "", 0 };

int main( void )
{
    FILE            *fp;
    unsigned        i;
    unsigned        j;
    unsigned        max_name;
    unsigned        len;
    machine_data    *mach;
    string_data     **insnames;
    ins_decode_data **decode;
    unsigned        *num_ins;
    int             *listl;

    fp = fopen( "distbls.gh", "w" );
    if( fp == NULL ) {
        fprintf( stderr, "can't open output file\n" );
        exit( 1 );
    }
    fprintf( fp, "#include <stddef.h>\n" );
    fprintf( fp, "/* file created by DISBUILD.C */\n\n" );
    AddString( &InvalidIns );
    AddString( &NullString );
    for( i = 0, mach = AMachine ; i < NUM_ELTS( AMachine ); ++i, ++mach ) {
        insnames = mach->ins_names;
        num_ins = mach->num_ins;
        max_name = sizeof( INVALID_INS ) - 1;
        for( ; *insnames != NULL ; ++insnames ) {
            for( j = 0; j < *num_ins; ++j ) {
                len = AddString( &(*insnames)[j] );
                if( len > max_name ) max_name = len;
            }
            ++num_ins;
        }
        fprintf( fp, "const unsigned char %sMaxInsName = %u;\n\n",
            mach->prefix, max_name );
        for( j = 0; j < mach->num_reg_names; ++j ) {
            AddString( &mach->reg_names[j] );
        }
        for( j = 0; j < mach->num_ref_names; ++j ) {
            AddString( &mach->ref_names[j] );
        }
    }
    BuildStringTable();
    fprintf( fp, "\nconst unsigned char DisStringTable[] = {\n" );
    for( i = 0; i < StringIndex; ++i ) {
        if( (i % 16) == 0 ) fprintf( fp, "/*%4.4x*/ ", i );
        if( StringTable[i] < ' ' || (StringTable[i] & LENGTH_BIT) ) {
            fprintf( fp, "%-3u,", StringTable[i] );
        } else {
            fprintf( fp, "'%c',", StringTable[i] );
        }
        if( (i % 16) == 15 ) fprintf( fp, "\n" );
    }
    fprintf( fp, "};\n\n" );
    for( i = 0, mach = AMachine ; i < NUM_ELTS( AMachine ); ++i, ++mach ) {
        num_ins = mach->num_ins;
        for( decode = mach->decode; *decode != NULL ; ++decode ) {
            for( j = 0; j < *num_ins; ++j ) {
                if( strcmp( (*decode)[j].handler, "NULL" ) != 0 ) {
                    fprintf( fp, "extern dis_handler_return %s( dis_handle *, void *, dis_dec_ins * );\n",
                        (*decode)[j].handler );
                }
            }
            ++num_ins;
        }
    }
    fprintf( fp, "\nconst dis_ins_descript DisInstructionTable[] = {\n" );
    fprintf( fp, "    { 0x%4.4x, 0x00000001, 0x00000000, NULL },\n", InvalidIns.string_idx );
    for( i = 0, mach = AMachine ; i < NUM_ELTS( AMachine ); ++i, ++mach ) {
        fprintf( fp, "\n    /* Machine:%s */\n\n", mach->prefix );
        insnames = mach->ins_names;
        num_ins = mach->num_ins;
        len = 1;
        for( decode = mach->decode; *decode != NULL ; ++decode ) {
            fprintf( fp, "    /* Table_%d */\n\n", len++ );
            for( j = 0; j < *num_ins; ++j ) {
                fprintf( fp, "    { 0x%4.4x, 0x%8.8lx, 0x%8.8lx, %s }, /* %s */\n",
                    (*insnames)[j].string_idx,
                    (unsigned long)(*decode)[j].opcode,
                    (unsigned long)(*decode)[j].mask,
                    (*decode)[j].handler,
                    (*decode)[j].idx_name );
            }
            fprintf( fp, "\n" );
            ++insnames;
            ++num_ins;
        }
    }
    fprintf( fp, "};\n" );
    fprintf( fp, "\nconst unsigned short DisRegisterTable[] = {\n" );
    fprintf( fp, "    0x%4.4x,\n", NullString.string_idx );
    for( i = 0, mach = AMachine ; i < NUM_ELTS( AMachine ); ++i, ++mach ) {
        for( j = 0; j < mach->num_reg_names; ++j ) {
            fprintf( fp, "    0x%4.4x,\n", mach->reg_names[j].string_idx );
        }
    }
    fprintf( fp, "};\n" );
    fprintf( fp, "\nconst unsigned short DisRefTypeTable[] = {\n" );
    fprintf( fp, "    0x%4.4x,\n", NullString.string_idx );
    for( i = 0, mach = AMachine ; i < NUM_ELTS( AMachine ); ++i, ++mach ) {
        for( j = 0; j < mach->num_ref_names; ++j ) {
            fprintf( fp, "    0x%4.4x,\n", mach->ref_names[j].string_idx );
        }
    }
    fprintf( fp, "};\n" );
    for( i = 0, mach = AMachine ; i < NUM_ELTS( AMachine ); ++i, ++mach ) {
        num_ins = mach->num_ins;
        j = 0;
        len = 0;
        listl = NULL;
        fprintf( fp, "\nconst dis_range %sRangeTable[] = {\n", mach->prefix );
        for( decode = mach->decode; *decode != NULL; ++decode ) {
            fprintf( fp, "\n    /* Table_%d */\n\n", ( len + 1 ) );
            listl = realloc( listl, ( len + 1 ) * sizeof( int ) );
            listl[ len++ ] = j;
            j += BuildRanges( fp, decode, num_ins, mach->prefix );
            ++num_ins;
        }
        fprintf( fp, "};\n" );
        fprintf( fp, "\nconst int %sRangeTablePos[] = {\n", mach->prefix );
        for( j = 0; j < len; ++j ) {
            fprintf( fp, "    %d,\n", listl[ j ] );
        }
        fprintf( fp, "    -1\n" );
        fprintf( fp, "};\n" );
        free( listl );
    }
    fprintf( fp, "\nconst dis_selector DisSelectorTable[] = {\n" );
    for( i = 0; i < SelIndex; ++i ) {
        if( (i % 16) == 0 ) fprintf( fp, "/*%4.4x*/", i );
        fprintf( fp, "%4d,", SelTable[i] );
        if( (i % 16) == 15 ) fprintf( fp, "\n" );
    }
    fprintf( fp, "};\n" );
    fclose( fp );
    return( 0 );
}
