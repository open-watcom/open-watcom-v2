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
* Description:  Internal definitions for dmpobj.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include "watcom.h"
#include "pcobj.h"
#include "bool.h"

typedef unsigned_8          byte;

#define CRLF            "\n"

#if defined( __QNX__ )
    #define OBJSUFFIX   ".o"
    #define LSTSUFFIX   ".lst"
#else
    #define OBJSUFFIX   ".obj"
    #define LSTSUFFIX   ".lst"
#endif

#define INDENT          "    "
#define INDWIDTH        (sizeof( INDENT ) - 1)
#define BAILOUT         CRLF INDENT "!! "


/*
    Output routines (output.c)
*/
extern  void            OutputInit( void );
extern  void            OutputFini( void );
extern  void            OutputSetFH( FILE * );
extern  size_t          Output( const char *fmt, ... );
extern  void            OutputData( unsigned_32 off, unsigned_32 len );

extern  int             no_disp;

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
   unsigned_16          segind;        /* into lnames                      */
} Segdeflist;

#define MAXGRPSEGS      64
typedef struct Grpdeflist {
   struct Grpdeflist    *next;
   unsigned_16          grpind;        /* into Lnames  for grpname         */
   unsigned_16          segidx[ MAXGRPSEGS ];  /* into segdefs for members */
} Grpdeflist;

extern  data_ptr        NamePtr;
extern  byte            NameLen;
extern  unsigned_16     RecLen;
extern  data_ptr        RecBuff;
extern  data_ptr        RecPtr;
extern  unsigned_16     RecMaxLen;
extern  unsigned_16     Grpindex;
extern  unsigned_16     Segindex;
extern  unsigned_16     Nameindex;
extern  unsigned_16     Importindex;
extern  unsigned_16     Libindex;
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
extern  unsigned_16     GetIndex( void );
extern  unsigned_32     GetVariable( void );
extern  void            ResizeBuff( unsigned_16 reqd_len );
extern  void            ProcFile( FILE *fp, bool );
extern  byte            RecNameToNumber( char *name );
extern  const char      *RecNumberToName( byte code );

extern  void            AddLname( void );
extern  char            *GetLname( unsigned_16 idx );
extern  void            AddSegdef( unsigned_16 idx );
extern  Segdeflist      *GetSegdef( unsigned_16 idx );
extern  void            AddGrpdef( unsigned_16 grpidx, unsigned_16 segidx );
extern  Grpdeflist      *GetGrpdef( unsigned_16 idx );
extern  unsigned_16     GetGrpseg( unsigned_16 idx );
extern  void            AddXname( void );
extern  char            *GetXname( unsigned_16 idx );
/*
    Record processing routines
*/
extern  void            ProcEndRec( void );
extern  void            ProcTHeadr( int );
extern  void            ProcLHeadr( void );
extern  void            ProcRHeadr( void );
extern  void            ProcComent( void );
extern  void            ProcNames( unsigned_16 * );
extern  void            ProcLNames( unsigned_16 * );
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
extern  void            leave( int );
extern  bool            Descriptions;
extern  bool            InterpretComent;
extern  bool            TranslateIndex;
