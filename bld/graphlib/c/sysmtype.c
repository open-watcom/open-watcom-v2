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


#include <conio.h>
#include "gdefn.h"
#include "montypes.h"
#include "gbios.h"
#include "svgadef.h"


//#if defined ( __386__ )
//    #define ID_SEG      0x0034
//    #define ID_OFF      0xFFFFE
//#else
//    #define ID_SEG      0xFFFF
//    #define ID_OFF      0x000E
//#endif
//#define ID_PCJR         0x0FD


#define MAX_DCC         0x0C
#define EGA_INF         0x10
#define PORT_MONO       0x003B4
#define PORT_COLOUR     0x003D4
#define PORT_STATUS     0x003BA


static short            CheckCGA( void );
static short            CheckMONO( void );
static short            ChkCursorReg( short );
static short            DCCEmulate( void );

extern void             Idle( void );
#pragma aux             Idle = 0xEB 0x00       /* jmp short *+2 */ \
                               0xEB 0x00       /* jmp short *+2 */ \
                               0xEB 0x00;      /* jmp short *+2 */


short _SysMonType( void )
/*=======================

    This routine returns the active monitor type plus an alternate
    type if it exists. */

{
    short           dcc;
    short           monitor_type;
    char            info;
    char            active_type;
    char            alternate_type;
//  char far *      p;

    dcc = VideoInt( _BIOS_VIDEO_DCC, 0, 0, 0 ) & 0x00ff;
    if( dcc == ( _BIOS_VIDEO_DCC >> 8 ) ) {       /* DCC function supported */
        monitor_type = VideoInt_bx( _BIOS_VIDEO_DCC, 0, 0, 0 );
        active_type = monitor_type & 0x00FF;                /* low byte     */
        alternate_type = monitor_type >> 8;                 /* high byte    */
        if( active_type <= MAX_DCC ) {              /* test for PS/2 series */
            if( active_type == MT_MDPA ) {
                active_type = CheckMONO();          /* MDPA or Hercules ?   */
            }
            if( alternate_type == MT_MDPA ) {
                alternate_type = CheckMONO();       /* MDPA or Hercules ?   */
            }
            if( active_type == MT_EGA_COLOUR ||
                alternate_type == MT_EGA_COLOUR ) {
                info = *(char far *)_BIOS_data( INFO_3 ) & 0x0F;
                if( info == 3 ) {
                    alternate_type = MT_EGA_ENHANCED;
                }
                if( info == 9 ) {
                    active_type = MT_EGA_ENHANCED;
                }
            }
#if defined( _SUPERVGA )
            if( active_type == MT_VGA_MONO || active_type == MT_VGA_COLOUR ) {
                if( _SuperVGAType() != _SV_NONE ) {
                    active_type += ( MT_SVGA_MONO - MT_VGA_MONO );
                }
            } else if( alternate_type == MT_VGA_MONO || alternate_type == MT_VGA_COLOUR) {
                if( _SuperVGAType() != _SV_NONE ) {
                    alternate_type = ( MT_SVGA_MONO - MT_VGA_MONO );
                }
            }
#endif
            return( ( alternate_type << 8 ) + active_type );
        }
    }
//  p = MK_FP( ID_SEG, ID_OFF );                        /* get machine ID   */
//  if( *p == ID_PCJR ) {                               /* PCjr detected    */
//      return( MT_JRCGA );
//  } else {
        return( DCCEmulate() );                 /* emulate a DCC function   */
//  }
}

static short DCCEmulate( void )
/*=============================

    First test the reserved switch settings for an EGA. If an EGA
    detected, get EGA information; otherwise assume a CGA. Valid
    EGA info is:    color range : 0-1 (colour,mono)
                    memory range : 0-3 (64K,128K,192K,256K).
    Otherwise assume a CGA. Check for alternate type.   */

{
    short           ega_color;
    short           ega_memory;
    char            info;
    char            active_type;
    char            alternate_type;
    char            video_mode;

    if( ( VideoInt_cx( _BIOS_ALT_SELECT, EGA_INF, 0, 0 ) & 0x00ff ) < 0x0C ) {
        ega_memory = EGA_Memory();
        ega_color = ega_memory >> 8;                        /* low byte     */
        ega_memory &= 0x00FF;                               /* high byte    */
        if( ega_color > 0x01 || ega_memory > 0x03 ) {
            active_type = CheckCGA();                   /* assume IBM CGA   */
            alternate_type = CheckMONO();
        } else {
            if( ega_color == 0 ) {                          /* EGA colour   */
                info = *(char far *)_BIOS_data( INFO_3 ) & 0x0F;
                if( info == 3 || info == 9 ) {
                    active_type = MT_EGA_ENHANCED;
                } else {
                    active_type = MT_EGA_COLOUR;
                }
                alternate_type = CheckMONO();
            } else {                                    /* EGA monochrome   */
                active_type = CheckCGA();
                alternate_type = MT_EGA_MONO;
            }
        }
    } else {                                            /* assume IBM CGA   */
        active_type = CheckCGA();
        alternate_type = CheckMONO();
    }
    /*  Swap active/alternate types for monochrome displays (modes 7,11,15) */
    video_mode = GetVideoMode();
    if( video_mode == 7 || video_mode == 11 || video_mode == 15 ) {
        return( ( active_type << 8 ) + alternate_type );
    } else {
        return( ( alternate_type << 8 ) + active_type );
    }
}


static short CheckMONO( void )
/*==========================

    Determine if monochrome adapter is a MDPA or HGC    */

{
    unsigned short      i;
    short               vert_sync;

    if( ChkCursorReg( PORT_MONO ) ) {               /* monochrome detected  */
        vert_sync = inp( PORT_STATUS ) & 0x0080;        /* isolate bit 7    */
        /*  Poll port many times to check for Hercules. Bit 7 of the
            vertical sync will be updated for an HGC.                       */
        for( i = 0; i < 32768; i++ ) {
            if( ( inp( PORT_STATUS ) & 0x0080 ) != vert_sync ) {    /* HGC  */
                vert_sync = inp( PORT_STATUS ) & 0x70;          /* detected */
                if( vert_sync == 0 ) {
                    return( MT_HERC );
                } else if( ( vert_sync & 0x10 ) == 0 ) {
                    return( MT_HERCPLUS );
                } else {
                    return( MT_HERCINCL );
                }
            }
        }                                               /* not a Hercules   */
        return( MT_MDPA );
    } else {
        return( FALSE );                                /* not a monochrome */
    }
}


static short CheckCGA( void )
/*===========================

    Check for the presence of an IBM CGA.   */

{
    if( ChkCursorReg( PORT_COLOUR ) ) {             /* CGA color detected   */
        return( MT_CGA_COLOUR );
    } else {
        return( FALSE );
    }
}


static short ChkCursorReg( short port )
/*=====================================

    Test the presence of a monochrome or color display  */

{
    short           cursor_pos;
    short           old_cursor_pos;

    outp( port, 0x0F );         /* CRT controller address of cursor at port */
    port++;                                         /* point to input port  */
    old_cursor_pos = inp( port );                   /* save current value   */
    outp( port, 0x5A );                         /* test with dummy value    */
    Idle();                                         /* slow it down a bit   */
    cursor_pos = inp( port );                       /* read cursor again    */
    outp( port, ( char ) old_cursor_pos );          /* reset old cursor     */
    return( cursor_pos == 0x5A );                   /* is cursor the same?  */

    /* Note: If the cursor remains the same, then the "port" indicates
             what it claims it indicates.   */
}
