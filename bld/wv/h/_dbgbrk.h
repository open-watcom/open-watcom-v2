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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*    enum          cmd             func             expr     */
pick( B_ACTIVATE,   "Activate\0",   ActivatePoint,   EXPR_CODE )
pick( B_CLEAR,      "Clear\0",      ClearPoint,      EXPR_CODE )
pick( B_DEACTIVATE, "Deactivate\0", DeactivatePoint, EXPR_CODE )
pick( B_SET,        "Set\0",        SetBreak,        EXPR_CODE )
pick( B_MODIFY,     "Modify\0",     SetWatch,        EXPR_DATA )
pick( B_TOGGLE,     "Toggle\0",     TogglePoint,     EXPR_CODE )
pick( B_RESUME,     "Resume\0",     ResumePoint,     EXPR_CODE )
pick( B_UNRESUME,   "UNResume\0",   UnResumePoint,   EXPR_CODE )
pick( B_INDEX,      "INdex\0",      BadPoint,        EXPR_DATA )
pick( B_IMAGE,      "IMage\0",      ImageBreak,      EXPR_DATA )
pick( B_UNMAPPED,   "Unmapped\0",   BadPoint,        EXPR_DATA )
pick( B_MAPADDRESS, "MAPaddress\0", BadPoint,        EXPR_DATA )
pick( B_SYMADDRESS, "SYMaddress\0", BadPoint,        EXPR_DATA )
