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


#ifndef death_h_included
#define death_h_included

/*
 * make sure that if you add a new way of dying, that you add a corresponding
 * string in util.cpp!
*/

enum CauseOfDeath {
    DEATH_BY_SPONTANEOUS_COMBUSTION,    // so valid exceptions are > 0
    DEATH_BY_FILE_READING,
    DEATH_BY_FILE_WRITING,
    DEATH_BY_FILE_SEEK,
    DEATH_BY_OUT_OF_MEMORY,
    DEATH_BY_MISSING_FILE,
    DEATH_BY_BAD_FILE,
    DEATH_BY_NO_FILES,
    DEATH_BY_ASSERTION,
    DEATH_BY_BAD_SIGNATURE,
    DEATH_BY_KILLER_DWARFS = 1000, // this for dwarf errors
    DEATH_BY_BAD_REGEXP = 2000     // this for problems with regular expressions
};

#endif
