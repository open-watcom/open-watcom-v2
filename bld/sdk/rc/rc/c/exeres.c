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


#include <io.h>
#include <string.h>
#include <ctype.h>
#include "watcom.h"
#include "exeos2.h"
#include "wresall.h"
#include "global.h"
#include "errors.h"
#include "types.h"
#include "rcmem.h"
#include "rcstr.h"
#include "exeutil.h"
#include "exeres.h"
#include "iortns.h"

extern void InitResTable( void )
/******************************/
{
    ExeResDir           *res;
    StringBlock         *str;
    WResDir             dir;

    res = &(Pass2Info.TmpFile.u.NEInfo.Res.Dir);
    str = &(Pass2Info.TmpFile.u.NEInfo.Res.Str);
    dir = Pass2Info.ResFiles->Dir;

    if (CmdLineParms.NoResFile) {
        res->NumTypes = 0;
        res->NumResources = 0;
        res->TableSize = 2 * sizeof(uint_16);
        /* the 2 uint_16 are the resource shift count and the type 0 record */
        res->Head = NULL;
        res->Tail = NULL;

        str->StringBlockSize = 0;
        str->StringBlock = NULL;
        str->StringListLen = 0;
        str->StringList = NULL;

    } else {
        res->NumTypes = WResGetNumTypes( dir );
        res->NumResources = WResGetNumResources( dir );
        res->TableSize = res->NumTypes * sizeof(resource_type_record) +
                            res->NumResources * sizeof(resource_record) +
                            2 * sizeof(uint_16);
        /* the 2 uint_16 are the resource shift count and the type 0 record */
        res->Head = NULL;
        res->Tail = NULL;

        StringBlockBuild( str, dir, FALSE );
    }
} /* InitResTable */

extern uint_32 ComputeResourceSize( WResDir dir )
/***********************************************/
{
    uint_32         length;
    WResDirWindow   wind;
    WResLangInfo    *res;

    length = 0;
    wind = WResFirstResource( dir );
    while( !WResIsEmptyWindow( wind ) ) {
        res = WResGetLangInfo( wind );
        length += res->Length;
        wind = WResNextResource( wind, dir );
    }
    return( length );
} /* ComputeResourceSize */

static uint_16 findResOrTypeName( ResTable * restab, WResID * name )
/******************************************************************/
{
    uint_16     name_id;
    int_32      str_offset;

    if (name->IsName) {
        str_offset = StringBlockFind( &restab->Str, &name->ID.Name );
        if (str_offset == -1 ) {
            name_id = 0;
        } else {
            name_id = str_offset + restab->Dir.TableSize;
        }
    } else {
        name_id = name->ID.Num | 0x8000;
    }

    return( name_id );
} /* findResOrTypeName */

static FullTypeRecord * addExeTypeRecord( ResTable * restab,
                            WResTypeInfo * type )
/**********************************************************/
{
    FullTypeRecord      *exe_type;

    exe_type = RcMemMalloc( sizeof(FullTypeRecord) );

    exe_type->Info.reserved = 0;
    exe_type->Info.num_resources = type->NumResources;
    exe_type->Info.type = findResOrTypeName( restab, &(type->TypeName) );
    exe_type->Head = NULL;
    exe_type->Tail = NULL;
    exe_type->Next = NULL;
    exe_type->Prev = NULL;

    /* use the general purpose linked list routines from WRes */
    ResAddLLItemAtEnd( (void **) &(restab->Dir.Head), (void **) &(restab->Dir.Tail), exe_type );

    return( exe_type );
} /* addExeTypeRecord */

static FullTypeRecord * findExeTypeRecord( ResTable * restab,
                            WResTypeInfo * type )
/***********************************************************/
{
    FullTypeRecord      *exe_type;
    StringItem16        *exe_type_name;

    for (exe_type = restab->Dir.Head; exe_type != NULL;
                exe_type = exe_type->Next) {
        if (type->TypeName.IsName && !(exe_type->Info.type & 0x8000)) {
            /* if they are both names */
            exe_type_name = (StringItem16 *) ((char *) restab->Str.StringBlock +
                            (exe_type->Info.type - restab->Dir.TableSize));
            if( exe_type_name->NumChars == type->TypeName.ID.Name.NumChars
                && !memicmp( exe_type_name->Name, type->TypeName.ID.Name.Name,
                             exe_type_name->NumChars ) ) break;
        } else if (!(type->TypeName.IsName) && exe_type->Info.type & 0x8000) {
            /* if they are both numbers */
            if (type->TypeName.ID.Num == (exe_type->Info.type & ~0x8000)) {
                break;
            }
        }
    }

    if (exe_type == NULL) {
        /* this is a new type */
        exe_type = addExeTypeRecord( restab, type );
    }

    return( exe_type );
} /* findExeTypeRecord */

static void addExeResRecord( ResTable *restab, FullTypeRecord *type,
                            WResID *name, uint_16 mem_flags,
                            uint_16 exe_offset, uint_16 exe_length )
/********************************************************************/
{
    FullResourceRecord          *exe_res;

    exe_res = RcMemMalloc( sizeof(FullResourceRecord) );

    exe_res->Info.offset = exe_offset;
    exe_res->Info.length = exe_length;
    exe_res->Info.flags = mem_flags;
    exe_res->Info.reserved = 0;
    exe_res->Info.name = findResOrTypeName( restab, name );
    exe_res->Next = NULL;
    exe_res->Prev = NULL;

    /* use the general purpose linked list routines from WRes */
    ResAddLLItemAtEnd( (void **) &(type->Head), (void **) &(type->Tail), exe_res );
} /* addExeResRecord */

static RcStatus copyOneResource( ResTable *restab, FullTypeRecord *type,
            WResLangInfo *lang, WResResInfo *res, int reshandle,
            int outhandle, int shift_count, int *err_code )
/****************************************************************************/
{
    RcStatus            error;
    int                 iorc;
    uint_32             out_offset;
    uint_32             align_amount;

    /* align the output file to a boundry for shift_count */
    error = RS_OK;
    out_offset = RcTell( outhandle );
    if( out_offset == -1 ) {
        error = RS_WRITE_ERROR;
        *err_code = errno;
    }
    if( error == RS_OK ) {
        align_amount = AlignAmount( out_offset, shift_count );
        iorc = RcSeek( outhandle, align_amount, SEEK_CUR );
        if( iorc == -1 ) {
            error = RS_WRITE_ERROR;
            *err_code = errno;
        }
        out_offset += align_amount;
    }

    if( error == RS_OK ) {
        iorc = RcSeek( reshandle, lang->Offset, SEEK_SET );
        if( iorc == -1 ) {
            error = RS_READ_ERROR;
            *err_code = errno;
        }
    }
    if( error == RS_OK ) {
        error = CopyExeData( reshandle, outhandle, lang->Length );
        *err_code = errno;
    }
    if( error == RS_OK ) {
        align_amount = AlignAmount( RcTell( outhandle ), shift_count );
        error = PadExeData( outhandle, align_amount );
        *err_code = errno;
    }

    if (!error) {
        addExeResRecord( restab, type, &(res->ResName), lang->MemoryFlags,
                out_offset >> shift_count,
                (lang->Length + align_amount) >> shift_count );
    }

    return( error );
} /* copyOneResource */

extern int CopyResources( uint_16 sect2mask, uint_16 sect2bits, bool sect2 )
/**************************************************************************/
/* Note: sect2 must be either 1 (do section 2) or 0 (do section 1) */
/* CopyResources should be called twice, once with sect2 false, and once with */
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
    int                 tmphandle;
    int                 reshandle;
    RcStatus            error;
    int                 err_code;

    dir = Pass2Info.ResFiles->Dir;
    restab = &(Pass2Info.TmpFile.u.NEInfo.Res);
    tmphandle = Pass2Info.TmpFile.Handle;
    reshandle = Pass2Info.ResFiles->Handle;
    error = RS_OK;

    /* walk through the WRes directory */
    wind = WResFirstResource( dir );
    while (!WResIsEmptyWindow( wind )) {
        if (WResIsFirstResOfType( wind )) {
            exe_type = findExeTypeRecord( restab, WResGetTypeInfo( wind ) );
        }

        res = WResGetResInfo( wind );
        lang = WResGetLangInfo( wind );

        /* if the bits are unequal and this is section 1 --> copy segment */
        /* if the bits are equal and this is section 2   --> copy segment */
        /* otherwise                                     --> do nothing */

        if (ARE_BITS_EQUAL( sect2mask, sect2bits, lang->MemoryFlags ) == sect2) {
            error = copyOneResource( restab, exe_type, lang, res, reshandle,
                                    tmphandle, restab->Dir.ResShiftCount,
                                    &err_code );
        }

        if( error != RS_OK ) break;

        CheckDebugOffset( &(Pass2Info.TmpFile) );

        wind = WResNextResource( wind, dir );
    }

    switch( error ) {
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_FILE, Pass2Info.TmpFile.name,
                 strerror( err_code ) );
        break;
    case RS_READ_ERROR:
        RcError( ERR_READING_RES, CmdLineParms.OutResFileName,
                 strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, CmdLineParms.OutResFileName );
        break;
    default:
        break;
    }
    return( error );
} /* CopyResources */

/*
 * writeTypeRecord-
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeTypeRecord( int handle, resource_type_record *res )
/******************************************************************/
{
    int     num_wrote;

    num_wrote = RcWrite( handle, res, sizeof(resource_type_record) );
    if( num_wrote != sizeof(resource_type_record) ) {
        return( RS_WRITE_ERROR );
    } else {
        return( RS_OK );
    }
} /* writeTypeRecord */

/*
 * writeResRecord-
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeResRecord( int handle, resource_record *type )
/******************************************************************/
{
    int     num_wrote;

    num_wrote = RcWrite( handle, type, sizeof(resource_record) );
    if( num_wrote != sizeof(resource_record) ) {
        return( RS_WRITE_ERROR );
    } else {
        return( RS_OK );
    }
} /* writeResRecord */

static void freeResTable( ResTable *restab )
/*******************************************/
{
    FullTypeRecord              *exe_type;
    FullTypeRecord              *old_type;
    FullResourceRecord          *exe_res;
    FullResourceRecord          *old_res;

    exe_type = restab->Dir.Head;
    while (exe_type != NULL) {
        exe_res = exe_type->Head;
        while (exe_res != NULL) {
            old_res = exe_res;
            exe_res = exe_res->Next;

            RcMemFree( old_res );
        }

        old_type = exe_type;
        exe_type = exe_type->Next;

        RcMemFree( old_type );
    }

    restab->Dir.Head = NULL;
    restab->Dir.Tail = NULL;
} /* freeResTable */

/*
 * writeStringBlock
 * NB when an error occurs this function must return without altering errno
 */
static int writeStringBlock( int handle, StringBlock *str )
/**********************************************************/
{
    int     numwrote;

    if( str->StringBlockSize > 0 ) {
        numwrote = RcWrite( handle, str->StringBlock, str->StringBlockSize );
        if( numwrote != str->StringBlockSize ) {
            return( RS_WRITE_ERROR );
        }
    }
    return( RS_OK );
} /* writeStringBlock */

/*
 * WriteResTable
 * NB when an error occurs this function must return without altering errno
 */
extern RcStatus WriteResTable( int handle, ResTable *restab, int *err_code )
/***************************************************************************/
{
    FullTypeRecord              *exe_type;
    FullResourceRecord          *exe_res;
    int                         num_wrote;
    int                         error;
    uint_16                     zero;

    error = RS_OK;
    num_wrote = RcWrite( handle, &(restab->Dir.ResShiftCount), sizeof(uint_16) );
    if( num_wrote != sizeof( uint_16 ) ) {
        error = RS_WRITE_ERROR;
    }

    for (exe_type = restab->Dir.Head; exe_type != NULL && error == RS_OK;
            exe_type = exe_type->Next) {
        error = writeTypeRecord( handle, &(exe_type->Info) );

        for( exe_res = exe_type->Head; exe_res != NULL && error == RS_OK;
                exe_res = exe_res->Next ) {
            error = writeResRecord( handle, &(exe_res->Info) );
        }
    }

    if( error ==  RS_OK ) {
        zero = 0;
        num_wrote = RcWrite( handle, &zero, sizeof(uint_16) );
        if( num_wrote != sizeof( uint_16 ) ) {
            error = RS_WRITE_ERROR;
        }
    }

    if( error == RS_OK ) {
        error = writeStringBlock( handle, &(restab->Str) );
    }
    *err_code = errno;
    freeResTable( restab );

    return( error );
} /* WriteResTable */
