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


#include "standard.h"
#include "coderep.h"
#include "vergen.h"
#include "pattern.h"

#define _ENC_GHW_CLASSIFY

static  bool    IsRXGenType( gentype generate ) {
/***********************************************/

    switch( generate ) {
    #define _ENC_GHW_RX( x ) \
    case G##x: return( TRUE );
    #include "ghw.h"
    #undef _ENC_GHW_RX
    }
    return( FALSE );
}

static  bool    IsRRGenType( gentype generate ) {
/***********************************************/

    switch( generate ) {
    #define _ENC_GHW_RR( x ) \
    case G##x: return( TRUE );
    #include "ghw.h"
    #undef _ENC_GHW_RR
    }
    return( FALSE );
}

static  bool    IsRSGenType( gentype generate ) {
/***********************************************/

    switch( generate ) {
    #define _ENC_GHW_RS( x ) \
    case G##x: return( TRUE );
    #include "ghw.h"
    #undef _ENC_GHW_RS
    }
    return( FALSE );
}


extern  bool    IsRXInstruction( instruction *ins ) {
/*******************************************/

    return( IsRXGenType( ins->u.gen_table->generate ) );
}

extern  bool    IsRRInstruction( instruction *ins ) {
/*******************************************/

    return( IsRRGenType( ins->u.gen_table->generate ) );
}

extern  bool    IsRSInstruction( instruction *ins ) {
/*******************************************/

    return( IsRSGenType( ins->u.gen_table->generate ) );
}

extern bool     ExpensiveIns( instruction *ins )
/*******************************************

*/
{
    switch( ins->u.gen_table->generate ) {
    case G_NO:
    case G_CLRR:
    case G_LTZ:
    case G_LTEZ:
    case G_LTDZ:
        return( FALSE );
    default:
        if( IsRSInstruction( ins ) ) return( FALSE );
        if( IsRRInstruction( ins ) ) return( FALSE );
        return( TRUE );
    }
}
