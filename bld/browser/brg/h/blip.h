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


#ifndef __BLIP_H__
#define __BLIP_H__

#ifndef STANDALONE_MERGER
  #include "browse.h"
#endif

class BlipCount {
public:
                    BlipCount( const char * str );
                    ~BlipCount();
    void            doBlip();
    void            start( const char * startMessage );
    void            end( const char * endMsg );
private:
    const char *    _str;           // string to put at front
    int             _count;         // total # of dies
    int             _blipCount;     // resetting counter
    int             _spinState;
    int             _spinPos;
    int             _spinDelta;

    const int       MaxSpin;        // max width on screen

#ifndef STANDALONE_MERGER
    #define             MAXBLIPBUFFER 100
    static char         _Buffer[ MAXBLIPBUFFER ];
#endif
};

#endif
