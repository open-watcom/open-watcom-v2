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


/*
    There are a number of types that a dip implementation must define.
    This file contains some example definitions.
*/
#ifndef DIP_SKEL
#define DIP_SKEL

#include <string.h>
#include "dip.h"
#include "dipimp.h"


/*
    An imp_mod_handle is defined as an unsigned_16. The value zero is
    reserved to indicate "no module".
*/

struct imp_sym_handle {
    /* any stuff for getting information on symbols */
    imp_mod_handle      im;
    void                *pointer;
};

struct imp_type_handle {
    /* any stuff for getting information on types */
    imp_mod_handle      im;
    void                *pointer;
};

struct imp_cue_handle {
    /* any stuff for getting information on source line cues */
    imp_mod_handle      im;
    void                *pointer;
};

/* Remember that that imp_[sym/type/cue]_handle's may be freely copied and
   destroyed by clients. No pointers to allocated memory unless there's
   another pointer somewhere else to free that memory. Clients don't have
   tell you how long they're hanging on to a handle either. Nasty bunch. */

struct imp_image_handle {
    /* This is the main structure that all other structures for an
       image (executable file) are hung off of. NO globals allowed.
       A single DIP may be used for multiple images. */
    struct section_info         *sect;
    unsigned                    num_sects;
    dig_fhandle                 sym_file;
    unsigned                    num_segs;
    char                        *lang;
};

extern address  NilAddr;

#endif
