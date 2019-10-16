/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include "cvars.h"
#include "target.h"
#include "intlload.h"

static IntlData *internationalData; // translated messages

//get default language
static char const  * const EMsgArray[] = {
    #include "msgtxt.gh"
};

//Enumerate groups
enum grp_index {
    grp_index_err = -1,
    #define GRP_DEF( name,prefix,num,index,eindex ) grp_index_##name,
    GRP_DEFS
    #undef GRP_DEF
    grp_index_max
};

//msg  index base
static int const LevelIndex[grp_index_max] = {
    #define GRP_DEF( name,prefix,num,index,eindex )  num,
    GRP_DEFS
    #undef GRP_DEF
};

//msg  index end
static int const LevelIndexEnd[grp_index_max] = {
    #define GRP_DEF( name,prefix,num,index,eindex )  num + eindex - index,
    GRP_DEFS
    #undef GRP_DEF
};

//msg ordinal base
static int const GroupBase[grp_index_max] = {
    #define GRP_DEF( name,prefix,num,index,eindex ) index,
    GRP_DEFS
    #undef GRP_DEF
};

static msg_type const MsgType[] = {
    #define MSG_DEF( name, group, kind, level, group_index )  MSG_TYPE_##kind,
    MSG_DEFS
    #undef MSG_DEF
};

static char const *MsgPrefix[grp_index_max] = {
    #define GRP_DEF( name,prefix,num,index,eindex ) #prefix,
    GRP_DEFS
    #undef GRP_DEF
};

static char const EUsage[] = {
    #include "usage.gh"
    "\0"
};

#ifndef __msg_file_prefix
#error no message file prefix
#endif

#define _mkstr( a ) #a
#define _str( a ) _mkstr( a )

void InitMsg( void )
{
    internationalData = LoadInternationalData( _str( __msg_file_prefix ) );
}

void FiniMsg( void )
{
    if( internationalData != NULL ) {
        FreeInternationalData( internationalData );
    }
}

char const *UsageText(void)      // GET INTERNATIONAL USAGE TEXT
{
    char const *usage_text;

    if( internationalData != NULL ) {
        usage_text = internationalData->usage_text;
    } else {
        usage_text = EUsage;
    }
    return( usage_text );
}

static enum grp_index  GetGrpIndex( msg_codes msgcode )
{
    enum grp_index   index;

    index = grp_index_max;
    while( index-- > 0 ) {
        if( msgcode >= LevelIndex[index] ) {
            if( msgcode < LevelIndexEnd[index] )
                return( index );
            break;
        }
    }
    return( grp_index_err );
}

int GetMsgIndex( msg_codes msgcode )
{
    enum grp_index   index;

    index = GetGrpIndex( msgcode );
    if( index == grp_index_err )
        return( -1 );
    return( msgcode - LevelIndex[index] + GroupBase[index] );
}

char const *CGetMsgPrefix( msg_codes msgcode )
{
    enum grp_index  index;

    index = GetGrpIndex( msgcode );
    if( index == grp_index_err )
        return( "" );
    return( MsgPrefix[index] );
}

msg_type CGetMsgType( msg_codes msgcode )
{
    int              msg_index;
    msg_type         kind;

    msg_index = GetMsgIndex( msgcode );
    kind = MsgType[msg_index];
    return( kind );
}

char const *CGetMsgStr( msg_codes msgcode )
{
    char const      *p;
    int             msg_index;

    msg_index = GetMsgIndex( msgcode );
    p = NULL;
    if( internationalData == NULL ) {
        p = EMsgArray[msg_index];
    } else if( msg_index < internationalData->errors_count ) {
        p = internationalData->errors_text[msg_index];
    }
    return( p );
}

void CGetMsg( char *msgbuf, msg_codes msgcode )
{
    char const *p = CGetMsgStr( msgcode );
    while( (*msgbuf++ = *p++) != '\0' )
        /* empty */;
}
