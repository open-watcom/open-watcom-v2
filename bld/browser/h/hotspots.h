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


#ifndef __HOTSPOTS_H__
#define __HOTSPOTS_H__

#define InvalidHotSpot         1
#define CheckBoxUncheckedU     2
#define CheckBoxUncheckedD     3
#define CheckBoxCheckedU       4
#define CheckBoxCheckedD       5

#define FunctionOpenUp         6
#define FunctionOpenDown       7
#define FunctionClosedUp       8
#define FunctionClosedDown     9

#define ClassOpenUp            10
#define ClassOpenDown          11
#define ClassClosedUp          12
#define ClassClosedDown        13

#define EnumOpenUp             14
#define EnumOpenDown           15
#define EnumClosedUp           16
#define EnumClosedDown         17

#define TypedefOpenUp          18
#define TypedefOpenDown        19
#define TypedefClosedUp        20
#define TypedefClosedDown      21

#define VariableOpenUp         22
#define VariableOpenDown       23
#define VariableClosedUp       24
#define VariableClosedDown     25

#define MacroOpenUp            26
#define MacroOpenDown          27
#define MacroClosedUp          28
#define MacroClosedDown        29

#define LabelOpenUp            30
#define LabelOpenDown          31
#define LabelClosedUp          32
#define LabelClosedDown        33

#define OutlineLeafUp          34
#define OutlineLeafDown        35
#define OutlineCollapsedUp     36
#define OutlineCollapsedDown   37
#define OutlineExpandedUp      38
#define OutlineExpandedDown    39

#define EditorHotUp            40
#define EditorHotDown          41

#define SplashPage             42

#define NumHotSpots            42

class WHotSpots;
extern WHotSpots * GlobalHotSpots;

#endif // __HOTSPOTS_H__
