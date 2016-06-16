/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DIP handles kind related declaration.
*
****************************************************************************/


/*      enum    DIP manager handle      DIP's imp.handle            WV internal                */
pick( HK_IMAGE, sizeof( image_handle ), sizeof( imp_image_handle ), 0                           )
pick( HK_TYPE,  sizeof( type_handle ),  sizeof( imp_type_handle ),  sizeof( imp_type_handle )   )
pick( HK_CUE,   sizeof( cue_handle ),   sizeof( imp_cue_handle ),   0                           )
pick( HK_SYM,   sizeof( sym_handle ),   sizeof( imp_sym_handle ),   sizeof( imp_sym_handle )    )
