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


#pragma pack(push,4)
typedef struct new_P5_timing_info {
    char        flag[4]; // the flag field MUST be first
    int lo_count;
    int hi_count;
    union {
        int     semaphore;
        struct new_P5_timing_info *dynamic;
    };
    int lo_cycle;
    int hi_cycle;
    int caller;
    int call_ins;
    int callee;
} new_P5_timing_info;

#pragma pack(pop)

extern "C" {
    extern void __PON( new_P5_timing_info * );
    extern void __POFF( new_P5_timing_info * );
    #pragma aux timerclass "*" parm routine [] modify []
    #pragma aux (timerclass) __PON;
    #pragma aux (timerclass) __POFF;
}

#define TimeData( data ) \
    static struct new_P5_timing_info __based( __segname( "TI" ) ) \
    data = { { 0,0,0,0 }, 0, 0, -1, 0, 0, 0, 0, 0  }

#define TimeStart( data ) __PON( &data )
#define TimeStop( data ) __POFF( &data )

#ifdef __cplusplus
    class __TimeAScope {
        public:
            __TimeAScope( new_P5_timing_info near *info )
            {
                _info = info;
                TimeStart( *_info );
            }
            ~__TimeAScope()
            {
                TimeStop( *_info );
            }
        private:
            new_P5_timing_info near *_info;
    };

    #define TimeScope( name ) \
        TimeData( name ); \
        __TimeAScope __timer( (new_P5_timing_info near *) &name );
#endif
