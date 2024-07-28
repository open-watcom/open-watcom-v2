/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OMF utility routines.
*
****************************************************************************/


//#define IMP_MODULENAME_DLL

#include "omfhash.h"

#define OMFHDRLEN   3
#define OMFSUMLEN   1

#define OMFRECORD(size) \
    struct { \
        unsigned_16 len; \
        unsigned_8  type; \
        unsigned_8  contents[size]; \
    }

#define INIT_OMF_REC_SIZE   1024
#define BLOCK_NAME_LEN      ( DIC_REC_SIZE - NUM_BUCKETS - 1 )

/*
 * Structs
 */
typedef OMFRECORD( 1 )  OmfRecord;

typedef struct{
    unsigned_8      htab[NUM_BUCKETS];
    unsigned_8      fflag;
    unsigned_8      name[BLOCK_NAME_LEN];
} OmfLibBlock;

extern void         InitOmfUtil( void );
extern void         WriteOmfRecHeader( libfile io, unsigned_8 type, unsigned_16 len );
extern unsigned     WriteOmfDict( libfile io, sym_file *first_sfile );
extern void         WriteOmfFile( libfile io, sym_file *sfile );
extern void         FiniOmfUtil( void );
