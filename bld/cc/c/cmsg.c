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


#include <stddef.h>
#include "target.h"
#include "intlload.h"
#include "cerrs.h"

static IntlData *internationalData; // translated messages
//get default language
static char const  * const EMsgArray[] = {
    #include "msgtxt.gh"
};
//Enumerate groups
enum grp_index {
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
//msg ordinal base
static int const GroupBase[grp_index_max] = {
#define GRP_DEF( name,prefix,num,index,eindex ) index,
    GRP_DEFS
#undef GRP_DEF
};
static msgtype const MsgType[] = {
#define MSG_DEF( name, group, kind, level, group_index )  msgtype_##kind,
    MSG_DEFS
#undef MSG_DEF
};
static char const *MsgPrefix[grp_index_max] = {
#define GRP_DEF( name,prefix,num,index,eindex ) #prefix,
    GRP_DEFS
#undef GRP_DEF
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
    if( internationalData != NULL ){
        FreeInternationalData( internationalData );
    }
}
static char const EUsage[] = {
#include "usage.gh"
"\0" };

char const * UsageText(void)      // GET INTERNATIONAL USAGE TEXT
{
    char const *usage_text;
    IntlData *data = internationalData;

    if( data != NULL ) {
        usage_text = data->usage_text;
    }else{
        usage_text = EUsage;
    }
    return( usage_text );
}

static enum grp_index  GetGrpIndex( msg_codes msgcode )
{
    enum grp_index   index;

    index = grp_index_max;
    while( --index > 0 ){
        if( msgcode >= LevelIndex[index] )break;
    }
    return( index );
}

char const *CGetMsgPrefix( msg_codes msgcode )
{
    enum grp_index   index;
    char   const    *value;

    index = GetGrpIndex( msgcode );
    value = MsgPrefix[index];
    return( value );
}

msgtype CGetMsgType( msg_codes msgcode )
{
    int              msgnum;
    msgtype          kind;
    enum grp_index   index;

    index = GetGrpIndex( msgcode );
    msgnum = msgcode - LevelIndex[index] + GroupBase[index];
    kind = MsgType[msgnum];
    return( kind );
}

char const *CGetMsgStr(  msg_codes msgcode )
{
    char const *p;
    int         msgnum;
    IntlData   *data = internationalData;
    enum grp_index   index;

    index = GetGrpIndex( msgcode );
    msgnum = msgcode - LevelIndex[index] + GroupBase[index];
    if( data != NULL ) {
        if( msgnum < (data->errors_count) ) {
            p = data->errors_text[ msgnum ];
        }
        else
            p = NULL;   // this might be a bug, but otherwise its random.
    }else{
        p = EMsgArray[ msgnum ];
    }
    return( p );
}

void CGetMsg( char *msgbuf, msg_codes msgcode )
{
    char const *p = CGetMsgStr( msgcode );
    while( (*msgbuf++ = *p++) )
        /* empty */;
}
