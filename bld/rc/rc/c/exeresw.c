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
* Description:  NE resource manipulation routines, Windows version.
*
****************************************************************************/


#include "global.h"
#include <errno.h>
#include "rcerrors.h"
#include "rcstrblk.h"
#include "rcstr.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exeutil.h"
#include "exeres.h"

#include "clibext.h"


void InitWINResTable( void )
/**************************/
{
    ExeResDir           *res;
    StringsBlock        *str;
    WResDir             dir;

    res = &(Pass2Info.TmpFile.u.NEInfo.Res.Dir);
    str = &(Pass2Info.TmpFile.u.NEInfo.Res.Str);
    dir = Pass2Info.ResFile->Dir;

    if( CmdLineParms.NoResFile ) {
        res->NumTypes = 0;
        /* the 2 uint_16 are the resource shift count and the type 0 record */
        res->TableSize = 2 * sizeof( uint_16 );
        res->Head = NULL;
        res->Tail = NULL;

        str->StringBlockSize = 0;
        str->StringBlock = NULL;
        str->StringListLen = 0;
        str->StringList = NULL;

    } else {
        res->NumTypes = WResGetNumTypes( dir );
        res->NumResources = WResGetNumResources( dir );
        /* the 2 uint_16 are the resource shift count and the type 0 record */
        res->TableSize = res->NumTypes * sizeof( resource_type_record ) +
                            res->NumResources * sizeof( resource_record ) +
                            2 * sizeof( uint_16 );
        res->Head = NULL;
        res->Tail = NULL;

        StringBlockBuild( str, dir, false );
    }
} /* InitWINResTable */


uint_32 ComputeWINResourceSize( WResDir dir )
/*******************************************/
{
    uint_32         length;
    WResDirWindow   wind;
    WResLangInfo    *res;

    length = 0;
    for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
        res = WResGetLangInfo( wind );
        length += res->Length;
    }
    return( length );
} /* ComputeWINResourceSize */


static uint_16 findResOrTypeName( ResTable *restab, WResID *name )
/****************************************************************/
{
    uint_16     name_id;
    int_32      str_offset;

    name_id = 0;
    if( name->IsName ) {
        str_offset = StringBlockFind( &restab->Str, &name->ID.Name );
        if( str_offset != -1 ) {
            name_id = str_offset + restab->Dir.TableSize;
        }
    } else {
        name_id = name->ID.Num | 0x8000;
    }

    return( name_id );
} /* findResOrTypeName */

static FullTypeRecord *addExeTypeRecord( ResTable *restab,
                            WResTypeInfo *type )
/********************************************************/
{
    FullTypeRecord      *exe_type;

    exe_type = RESALLOC( sizeof( FullTypeRecord ) );

    exe_type->Info.reserved = 0;
    exe_type->Info.num_resources = type->NumResources;
    exe_type->Info.type = findResOrTypeName( restab, &(type->TypeName) );
    exe_type->Head = NULL;
    exe_type->Tail = NULL;
    exe_type->Next = NULL;
    exe_type->Prev = NULL;

    /* use the general purpose linked list routines from WRes */
    ResAddLLItemAtEnd( (void **)&(restab->Dir.Head), (void **)&(restab->Dir.Tail), exe_type );

    return( exe_type );
} /* addExeTypeRecord */

static FullTypeRecord *findExeTypeRecord( ResTable *restab,
                            WResTypeInfo *type )
/*********************************************************/
{
    FullTypeRecord      *exe_type;
    StringItem16        *exe_type_name;

    for( exe_type = restab->Dir.Head; exe_type != NULL; exe_type = exe_type->Next ) {
        if( type->TypeName.IsName && (exe_type->Info.type & 0x8000) == 0 ) {
            /* if they are both names */
            exe_type_name = (StringItem16 *)((char *)restab->Str.StringBlock
                               + (exe_type->Info.type - restab->Dir.TableSize));
            if( exe_type_name->NumChars == type->TypeName.ID.Name.NumChars
              && strnicmp( exe_type_name->Name, type->TypeName.ID.Name.Name, exe_type_name->NumChars ) == 0 ) {
                break;
            }
        } else if( !(type->TypeName.IsName) && (exe_type->Info.type & 0x8000) ) {
            /* if they are both numbers */
            if( type->TypeName.ID.Num == (exe_type->Info.type & ~0x8000) ) {
                break;
            }
        }
    }

    if( exe_type == NULL ) {
        /* this is a new type */
        exe_type = addExeTypeRecord( restab, type );
    }

    return( exe_type );
} /* findExeTypeRecord */

static void addExeResRecord( ResTable *restab, FullTypeRecord *type,
                            WResID *name, uint_16 mem_flags,
                            uint_16 exe_offset, uint_16 exe_length )
/******************************************************************/
{
    FullResourceRecord          *exe_res;

    exe_res = RESALLOC( sizeof( FullResourceRecord ) );

    exe_res->Info.offset = exe_offset;
    exe_res->Info.length = exe_length;
    exe_res->Info.flags = mem_flags;
    exe_res->Info.reserved = 0;
    exe_res->Info.name = findResOrTypeName( restab, name );
    exe_res->Next = NULL;
    exe_res->Prev = NULL;

    /* use the general purpose linked list routines from WRes */
    ResAddLLItemAtEnd( (void **)&(type->Head), (void **)&(type->Tail), exe_res );
} /* addExeResRecord */

static RcStatus copyOneResource( ResTable *restab, FullTypeRecord *type,
            WResLangInfo *lang, WResResInfo *res, FILE *res_fp,
            FILE *out_fp, int shift_count, int *err_code )
/**********************************************************************/
{
    RcStatus            ret;
    long                out_offset;
    long                align_amount;

    /* align the output file to a boundary for shift_count */
    ret = RS_OK;
    align_amount = 0;   // shut up gcc
    out_offset = RESTELL( out_fp );
    if( out_offset == -1 ) {
        ret = RS_WRITE_ERROR;
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        align_amount = AlignAmount( out_offset, shift_count );
        if( RESSEEK( out_fp, align_amount, SEEK_CUR ) ) {
            ret = RS_WRITE_ERROR;
            *err_code = errno;
        }
        out_offset += align_amount;
    }

    if( ret == RS_OK ) {
        if( RESSEEK( res_fp, lang->Offset, SEEK_SET ) ) {
            ret = RS_READ_ERROR;
            *err_code = errno;
        }
    }
    if( ret == RS_OK ) {
        ret = CopyExeData( res_fp, out_fp, lang->Length );
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        align_amount = AlignAmount( RESTELL( out_fp ), shift_count );
        ret = PadExeData( out_fp, align_amount );
        *err_code = errno;
    }

    if( ret == RS_OK ) {
        addExeResRecord( restab, type, &(res->ResName), lang->MemoryFlags,
                out_offset >> shift_count, (lang->Length + align_amount) >> shift_count );
    }

    return( ret );
} /* copyOneResource */

RcStatus CopyWINResources( uint_16 sect2mask, uint_16 sect2bits, bool sect2 )
/***************************************************************************/
/* Note: sect2 must be either 1 (do section 2) or 0 (do section 1) */
/* CopyWINResources should be called twice, once with sect2 false, and once with */
/* it true. The values of sect2mask and sect2bits should be the same for both */
/* calls. The resource table for the temporary file will not be properly */
/* filled in until after the second call */
{
    WResDir             dir;
    WResDirWindow       wind;
    ResTable            *restab;
    FullTypeRecord      *exe_type;
    WResResInfo         *res;
    WResLangInfo        *lang;
    FILE                *tmp_fp;
    FILE                *res_fp;
    RcStatus            ret;
    int                 err_code;

    dir = Pass2Info.ResFile->Dir;
    restab = &(Pass2Info.TmpFile.u.NEInfo.Res);
    tmp_fp = Pass2Info.TmpFile.fp;
    res_fp = Pass2Info.ResFile->fp;
    ret = RS_OK;
    err_code = 0;

    /* walk through the WRes directory */
    exe_type = NULL;
    for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
        if( WResIsFirstResOfType( wind ) ) {
            exe_type = findExeTypeRecord( restab, WResGetTypeInfo( wind ) );
        }

        res = WResGetResInfo( wind );
        lang = WResGetLangInfo( wind );

        /* if the bits are unequal and this is section 1 --> copy segment */
        /* if the bits are equal and this is section 2   --> copy segment */
        /* otherwise                                     --> do nothing */

        if( ARE_BITS_EQUAL( sect2mask, sect2bits, lang->MemoryFlags ) == sect2 ) {
            ret = copyOneResource( restab, exe_type, lang, res, res_fp,
                                    tmp_fp, restab->Dir.ResShiftCount, &err_code );
        }

        if( ret != RS_OK )
            break;

        CheckDebugOffset( &(Pass2Info.TmpFile) );
    }

    switch( ret ) {
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_FILE, Pass2Info.TmpFile.name, strerror( err_code ) );
        break;
    case RS_READ_ERROR:
        RcError( ERR_READING_RES, CmdLineParms.OutResFileName, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, CmdLineParms.OutResFileName );
        break;
    default:
        break;
    }
    return( ret );
} /* CopyWINResources */


/*
 * writeTypeRecord-
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeTypeRecord( FILE *fp, resource_type_record *res )
/********************************************************************/
{
    if( RESWRITE( fp, res, sizeof( resource_type_record ) ) != sizeof( resource_type_record ) ) {
        return( RS_WRITE_ERROR );
    } else {
        return( RS_OK );
    }
} /* writeTypeRecord */


/*
 * writeResRecord-
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeResRecord( FILE *fp, resource_record *type )
/***************************************************************/
{
    if( RESWRITE( fp, type, sizeof( resource_record ) ) != sizeof( resource_record ) ) {
        return( RS_WRITE_ERROR );
    } else {
        return( RS_OK );
    }
} /* writeResRecord */


static void freeResTable( ResTable *restab )
/******************************************/
{
    FullTypeRecord              *exe_type;
    FullTypeRecord              *next_type;
    FullResourceRecord          *exe_res;
    FullResourceRecord          *next_res;

    for( exe_type = restab->Dir.Head; exe_type != NULL; exe_type = next_type ) {
        next_type = exe_type->Next;
        for( exe_res = exe_type->Head; exe_res != NULL; exe_res = next_res ) {
            next_res = exe_res->Next;
            RESFREE( exe_res );
        }
        RESFREE( exe_type );
    }

    restab->Dir.Head = NULL;
    restab->Dir.Tail = NULL;
} /* freeResTable */


/*
 * writeStringBlock
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeStringBlock( FILE *fp, StringsBlock *str )
/*************************************************************/
{
    if( str->StringBlockSize > 0 ) {
        if( RESWRITE( fp, str->StringBlock, str->StringBlockSize ) != str->StringBlockSize ) {
            return( RS_WRITE_ERROR );
        }
    }
    return( RS_OK );
} /* writeStringBlock */

RcStatus WriteWINResTable( FILE *fp, ResTable *restab, int *err_code )
/********************************************************************/
/*
 * WriteWINResTable
 * NB when an error occurs this function must return without altering errno
 */
{
    FullTypeRecord              *exe_type;
    FullResourceRecord          *exe_res;
    RcStatus                    ret;
    uint_16                     zero;

    ret = RS_OK;
    if( RESWRITE( fp, &(restab->Dir.ResShiftCount), sizeof( uint_16 ) ) != sizeof( uint_16 ) ) {
        ret = RS_WRITE_ERROR;
    }

    for( exe_type = restab->Dir.Head; exe_type != NULL && ret == RS_OK;
            exe_type = exe_type->Next ) {
        ret = writeTypeRecord( fp, &(exe_type->Info) );

        for( exe_res = exe_type->Head; exe_res != NULL && ret == RS_OK;
                exe_res = exe_res->Next ) {
            ret = writeResRecord( fp, &(exe_res->Info) );
        }
    }

    if( ret == RS_OK ) {
        zero = 0;
        if( RESWRITE( fp, &zero, sizeof( zero ) ) != sizeof( zero ) ) {
            ret = RS_WRITE_ERROR;
        }
    }

    if( ret == RS_OK ) {
        ret = writeStringBlock( fp, &(restab->Str) );
    }
    *err_code = errno;
    freeResTable( restab );

    return( ret );
} /* WriteWINResTable */
