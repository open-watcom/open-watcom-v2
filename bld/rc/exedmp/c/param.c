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


#include "param.h"

extern const char *resTypes[];

#define TOGGLE( x )     (x) = !(x)

void defaultParam( Parameters* param )
/*************************************/
{
    param->dumpOffsets          = DEF_DUMPOFFSETS;
    param->dumpHexHeaders       = DEF_DUMPHEXHEADERS;
    param->dumpHexData          = DEF_DUMPHEXDATA;
    param->dumpInterpretation   = DEF_DUMPINTERPRETATION;
    param->dumpExeHeaderInfo    = DEF_DUMPEXEHEADERINFO;
    param->dumpResObjectInfo    = DEF_DUMPRESOBJECTINFO;
    param->printRuler           = DEF_PRINTRULER;
    param->specificType         = DEF_SPECIFICTYPE;
    param->specificTypeID       = DEF_SPECIFICTYPEID;
    param->indentSpaces         = DEF_INDENTSPACES;
    param->hexIndentSpaces      = DEF_HEXINDENTSPACES;
    param->filename             = NULL;
}


bool loadParam( Parameters *param, int count, char *params[] )
/*************************************************************/
{
    int i;
    unsigned_16 j;
    bool unknownParam;

    unknownParam = false;
    for( i = 1; i < count; i++ ) {
        if( params[i][0] == '-' || params[i][0] == '/' ) {
            switch( params[i][1] ) {
            case OPT_DUMPHEXHEADERS:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->dumpHexHeaders );
                break;
            case OPT_DUMPOFFSETS:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->dumpOffsets );
                break;
            case OPT_DUMPHEXDATA:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->dumpHexData );
                break;
            case OPT_DUMPINTERPRETATION:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->dumpInterpretation );
                break;
            case OPT_DUMPEXEHEADERINFO:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->dumpExeHeaderInfo );
                break;
            case OPT_DUMPRESOBJECTINFO:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->dumpResObjectInfo );
                break;
            case OPT_PRINTRULER:
                if( params[i][2] != '\0' ) unknownParam = true;
                else                       TOGGLE( param->printRuler );
                break;
            case OPT_INDENTSPACES:
                if( params[i][2] == '\0' ) {
                    printf( ERR_PARAM_INDENTSPACES );
                    return( false );
                } else {
                    param->indentSpaces = atoi( &params[i][2] );
                    if( param->indentSpaces < 0 ) {
                        param->indentSpaces = 0;
                    }
                }
                break;
            case OPT_HEXINDENTSPACES:
                if( params[i][2] == '\0' ) {
                    printf( ERR_PARAM_HEXINDENTSPACES );
                    return( false );
                } else {
                    param->hexIndentSpaces = atoi( &params[i][2] );
                    if( param->hexIndentSpaces < -1 ) {
                        param->hexIndentSpaces = -1;
                    }
                }
                break;
            case OPT_SPECIFICTYPE:
                if( params[i][2] == '\0' ) {
                    printf( ERR_PARAM_SPECIFICTYPE );
                    return( false );
                } else {
                    for( j = 0; resTypes[j] != NULL; j++ ) {
                        if( stricmp( resTypes[j], &params[i][2] ) == 0 ) {
                            param->specificType = true;
                            param->specificTypeID = j;
                            break;
                        }
                    }
                    if( resTypes[j] == NULL ) {
                        printf( ERR_PARAM_SPECIFICTYPE_UNKNOWN );
                        return( false );
                    }
                }
                break;
            default:
                unknownParam = true;
            }

            if( unknownParam ) {
                printf( ERR_PARAM_UNKNOWN, params[i] );
                return( false );
            }
        } else {
            param->filename = (char *) malloc( strlen( params[i] ) + 1 );
            if( param->filename != NULL ) {
                strcpy( param->filename, params[i] );
            }
        }
    }
    return( true );
}
