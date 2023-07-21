/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal definitions for dmpobj.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include "watcom.h"
#include "pcobj.h"
#include "bool.h"
#include "wnoret.h"


#define INDENT          "    "
#define INDWIDTH        (sizeof( INDENT ) - 1)
#define BAILOUT         "\n" INDENT "!! "

typedef unsigned_8      byte;

/*
    Output routines (output.c)
*/
extern  void            OutputInit( void );
extern  void            OutputFini( void );
extern  void            OutputSetFH( FILE * );
extern  size_t          Output( const char *fmt, ... );
extern  size_t          OutputNL( void );
extern  void            OutputData( unsigned_32 off, unsigned_32 len );

extern  bool            no_disp;

/*
    Object reading (objread.c)
*/
typedef byte            *data_ptr;

typedef struct Lnamelist {
   struct Lnamelist    *next;
   byte                 LnameLen;
   byte                 Lname; // really LnameLen bytes ( can be ZERO )
} Lnamelist;

typedef struct Segdeflist {
   struct Segdeflist    *next;
   unsigned             segidx;        /* into lnames                      */
} Segdeflist;

#define MAXGRPSEGS      64
typedef struct Grpdeflist {
   struct Grpdeflist    *next;
   unsigned             grpidx;        /* into Lnames  for grpname         */
   unsigned             segidx[ MAXGRPSEGS ];  /* into segdefs for members */
} Grpdeflist;

extern  data_ptr        NamePtr;
extern  byte            NameLen;
extern  unsigned_16     RecLen;
extern  data_ptr        RecBuff;
extern  data_ptr        RecPtr;
extern  unsigned_16     RecMaxLen;
extern  unsigned        Grpindex;
extern  unsigned        Segindex;
extern  unsigned        Nameindex;
extern  unsigned        Importindex;
extern  unsigned        Libindex;
extern  bool            IsPharLap;
extern  bool            IsMS386;
extern  bool            IsIntel;
extern  bool            DumpRaw;
extern  jmp_buf         BailOutJmp;
extern  char            ProvideRawTypes[];
extern  char            OmitTypes[];
extern  bool            IgnoreOutput;
extern  byte            rec_type[];
extern  int             rec_count;

extern  bool            EndRec( void );
extern  unsigned_16     RecOffset( void );
extern  byte            GetByte( void );
extern  void            BackupByte( void );
extern  unsigned_16     GetUInt( void );
extern  unsigned_32     GetLInt( void );
extern  unsigned_32     GetEither( void );
extern  byte            GetName( void );       /* length prefixed name     */
extern  unsigned        GetIndex( void );
extern  unsigned_32     GetVariable( void );
extern  void            ResizeBuff( unsigned_16 reqd_len );
extern  void            ProcFile( FILE *fp, bool );
extern  byte            RecNameToNumber( char *name );
extern  const char      *RecNumberToName( byte code );

extern  void            AddLname( void );
extern  char            *GetLname( unsigned idx );
extern  void            AddSegdef( unsigned idx );
extern  Segdeflist      *GetSegdef( unsigned idx );
extern  void            AddGrpdef( unsigned grpidx, unsigned segidx );
extern  Grpdeflist      *GetGrpdef( unsigned idx );
extern  unsigned        GetGrpseg( unsigned idx );
extern  void            AddXname( void );
extern  char            *GetXname( unsigned idx );
/*
    Record processing routines
*/
extern  void            ProcEndRec( void );
extern  void            ProcTHeadr( int );
extern  void            ProcLHeadr( void );
extern  void            ProcRHeadr( void );
extern  void            ProcComent( void );
extern  void            ProcNames( unsigned * );
extern  void            ProcLNames( unsigned * );
extern  void            ProcExtNames( void );
extern  void            ProcModEnd( void );
extern  void            ProcSegDefs( void );
extern  void            ProcLocSyms( void );
extern  void            ProcPubDefs( void );
extern  void            ProcComDef( void );
extern  void            ProcLinNums( void );
extern  void            ProcLidata( void );
extern  void            ProcLedata( void );
extern  void            ProcFixup( void );
extern  void            ProcGrpDef( void );
extern  void            ProcLibHeader( void );
extern  void            ProcLibTrailer( FILE *fp );
extern  void            ProcBackPat( void );
extern  void            ProcNameBackPat( void );
extern  void            ProcComExtDef( void );
extern  void            ProcComDat( void );
extern  void            ProcLineSym( void );
extern  void            ProcAlias( void );
extern  void            ProcVerNum( void );
extern  void            ProcVendExt( void );

/*
    Misc routines (dmpobj.c)
*/
NO_RETURN( extern void  leave( int ) );
extern  bool            Descriptions;
extern  bool            InterpretComent;
extern  bool            TranslateIndex;
