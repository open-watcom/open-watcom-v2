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


#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"


boolean CgRetnOptForFile        // TEST IF RETURN OPTIMIZATION FOR FILE
    ( CGFILE* file_ctl )        // - file control
{
    return file_ctl->s.opt_retn_val || file_ctl->s.opt_retn_ref;
}


boolean CgRetnOptActive         // TEST IF RETURN OPTIMIZATION ACTIVE FOR FILE
    ( FN_CTL* fctl )            // - file-activation control
{
    CGFILE* file_ctl;           // - file control

    file_ctl = fctl->cgfile;
    return CgRetnOptForFile( file_ctl )
        && !( fctl->debug_info && ( GenSwitches & DBG_LOCALS ) );
}


boolean CgRetnOptIsOptVar       // TEST IF SYMBOL IS RETURN-OPTIMIZATION VAR
    ( FN_CTL* fctl              // - file-activation control
    , SYMBOL var )              // - var. to be tested
{
    return CgRetnOptActive( fctl )
        && var == fctl->cgfile->opt_retn;

}


void CgRetnOpt_RETNOPT_BEG      // PROCESS IC_RETNOPT_BEG
    ( FN_CTL* fctl )            // - file-activation control
{
    if( CgRetnOptActive( fctl ) ) {
        CgioReadICUntilOpcode( fctl->cgfile, IC_RETNOPT_END );
    }
}


// NOTE: this is only processed by back-end
//
// We cannot easily determine whether a function will optimize away the
// return during call-graph processing, so we must be conservative.
//
// Codes are skipped when "var" is optimized away.
//
void CgRetnOpt_RETNOPT_VAR      // PROCESS IC_RETNOPT_VAR
    ( FN_CTL* fctl              // - file-activation control
    , SYMBOL var )              // - variable
{
    CGFILE* file_ctl;           // - file control

    file_ctl = fctl->cgfile;
    if( var == file_ctl->opt_retn
     && CgRetnOptActive( fctl ) ) {
        CgioReadICUntilOpcode( file_ctl, IC_RETNOPT_END );
    }
}
