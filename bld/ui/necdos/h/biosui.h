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


#define BIOS_VIDEO          0x18
#define BIOS_EXT_VIDEO      0xDC
#define BIOS_MOUSE          0x33
#define SYSTEM_CLOCK        0x6C

#ifdef __WINDOWS__
#define BIOS_PAGE           0x40
#else
#define BIOS_PAGE           0x70
#endif

#define UNDEFINED           3

#define REAL_SEGMENT        0x34
#define ROW_OFFSET          0x10
#define COL_OFFSET          0x1C

#define SCREEN_OFFSET       0x4e

#define NEC_20_LINES        0x01
#define NEC_40_COLS         0x02
#define NEC_GRAPHIC         0x04
#define NEC_KCG_DOT         0x08
#define NEC_31_LINES        0x10
#define NEC_NORMHI_MODE     0x80

#define NEC_25_LINES        0x00
#define NEC_80_COLS         0x00
#define NEC_VER_LINE        0x00
#define NEC_KCG_CODE        0x00

/**********************
  system level pragmas
 **********************/

#pragma aux IdleInterrupt = \
 0xCD 0x28;         /* int     028 */

extern void        IdleInterrupt(void);


#pragma aux BIOSSetMode =                               \
    "mov    ah,0ah                              "       \
    "int    18h                                 "       \
        parm caller [ ax ];

extern void        BIOSSetMode(unsigned);


#pragma aux BIOSGetMode =                               \
    "mov    ah,0bh                              "       \
    "int    18h                                 "       \
        value       [ al ];

extern char        BIOSGetMode(void);


#pragma aux BIOS_SHF_JIS =                              \
    "mov    cl, 0xf4                            "       \
    "int    0DCh                                "       \
        parm caller [ ax ]                              \
        modify      [ ax cl ]                           \
        value       [ ax ];

extern unsigned short BIOS_SHF_JIS( unsigned short );


/***************************
  CRT Consol Output pragmas
 ***************************/

#pragma aux BIOSClrVRAM =                               \
    "mov    ah, 16h                             "       \
    "int    18h                                 "       \
        parm caller [ dh ] [ dl ]                       \
        modify [ ah dx ];

extern void BIOSClrVRAM( unsigned char, unsigned char );

#pragma aux BIOSInitCrt = \
    "mov ax, 1CCCh" \
    "int 18h"\
    modify [ax];
extern void BIOSInitCrt();

#pragma aux BIOSClrScreen =                             \
    "mov    cl, 10h                             "       \
    "mov    ah, 0Ah                             "       \
    "mov    dl, 02h                             "       \
    "int    0DCh                                "       \
        modify [ cl ah dl ];

extern void BIOSClrScreen(void);


#pragma aux BIOSSetCursorType =                         \
    "mov    ah, 10h                             "       \
    "int    18h                                 "       \
        parm caller [ al ]                              \
        modify [ ax ];

extern void BIOSSetCursorType( unsigned char );


#pragma aux BIOSSetCursorOn =                           \
    "mov    ah, 11h                             "       \
    "int    18h                                 "       \
        modify [ ah ];

extern void BIOSSetCursorOn( void );


#pragma aux BIOSSetCursorOff =                          \
    "mov    ah, 12h                             "       \
    "int    18h                                 "       \
        modify [ ah ];

extern void BIOSSetCursorOff( void );


#pragma aux BIOSSetCurPos =                             \
    "mov    ah, 13h                             "       \
    "int    18h                                 "       \
        parm caller [ dx ]                              \
        modify [ ah ];

extern void BIOSSetCurPos( int );


#pragma aux GdcMask =                                   \
    "pushf                                      "       \
    "cli                                        "

void GdcMask( void );


#pragma aux GdcUnMask =                                 \
        "popf"

void GdcUnMask( void );


#pragma aux GdcDelay =                                  \
    "       jmp short dl1                       "       \
    "dl1:   jmp short dl2                       "       \
    "dl2:   jmp short dl3                       "       \
    "dl3:                                       "

void GdcDelay( void );


#pragma aux BIOSSetAttr =                               \
    "mov    cl,10h                              "       \
    "mov    ah,02h                              "       \
    "int    0DCh                                "       \
        parm caller [ dl ]                              \
        modify [ ax cx ];

extern void        BIOSSetAttr(unsigned char);


#pragma aux BIOSSendData =                              \
    "mov    cl,10h                              "       \
    "mov    ah,00h                              "       \
    "int    0DCh                                "       \
        parm caller [ dl ]                              \
        modify [ ax cx ];

extern void        BIOSSendData(unsigned char);


/***********************
  Keyboard BIOS pragmas
 ***********************/

#pragma aux BIOSInitKeyboard =                          \
    "mov    ah, 03h                             "       \
    "int    18h                                 "       \
    "mov    ah, 06h                             "       \
    "int    18h                                 "       \
        modify [ ax ];

extern void BIOSInitKeyboard(void);


#pragma aux BIOSGetKeyboard =                           \
    "int     18h                                "       \
        parm caller [ ah ]                              \
        value       [ ax ]                              \
        modify      [ bx ];

extern short BIOSGetKeyboard( char );


#pragma aux BIOSKeyboardHit =                           \
    "       int     18h                         "       \
    "       test    bh, 01h                     "       \
    "       jz      foo1                        "       \
    "       jmp     short foo2                  "       \
    "foo1:  mov     ax, 0                       "       \
    "foo2:                                      "       \
        parm caller [ ah ]                              \
        value       [ ax ]                              \
        modify      [ bx ]

extern short BIOSKeyboardHit( char );


#pragma aux BIOSSenseShift =                            \
    "mov    ah, 02h                             "       \
    "int    18h                                 "       \
        value       [ al ];

extern char BIOSSenseShift( void );


/********************
  Mouse BIOS pragams
 ********************/
#if 0
#pragma aux BIOSNormLeftMouse =                         \
    "mov    ax, 05h                             "       \
    "int    33h                                 "       \
        modify  [ bx cx dx ]                            \
        value   [ ax ];

extern short BIOSNormLeftMouse( void );


#pragma aux BIOSNormRightMouse =                        \
    "mov    ax, 07h                             "       \
    "int    33h                                 "       \
        modify  [ bx cx dx ]                            \
        value   [ ax ];

extern short BIOSNormRightMouse( void );


#pragma aux BIOSHighMouse =                             \
    "mov    ax, 05h                             "       \
    "mov    bx, 00h                             "       \
    "int    33h                                 "       \
        modify  [ bx cx dx ]                            \
        value   [ ax ];

extern short BIOSHighMouse( void );
#endif


extern void uisetcurrpage( int );
extern int CurrentPageNum( );

/*
 * Originally, the code for the nec screen updates was written using the
 * PIXEL structure, which conviniently had two bytes to poke into memory.
 * However - just because a structure happens to have the same size, there
 * is absolutely no reason to reuse it.  Defining a new type does not eat
 * any extra memory in the end.  Here is the new structure which lets us
 * access the NEC screen.
 */

typedef struct code_screen {
    char left;
    char right;
} ZEN_CODE, __far * LP_ZEN_CODE;

typedef struct attr_screen {
    char left;
    char right;
} ZEN_ATTR, __far * LP_ZEN_ATTR;

/*
 * Knowledge of Box-Drawing characters extends to two locations.
 * Here, I include the prefix of all such characters used in UI.
 */
#define ZEN_BOX_PREFIX 0x0B

/*
 * The following section deals with the treatment of double-byte
 * characters.
 */

#define ZEN_NOTHING_IN_PARTICULAR       0
#define ZEN_DB_PREFIX                   1
#define ZEN_DB_SUFFIX                   2
#define ZEN_BOX_CHAR                    3

/*
 * For a description of this function, see uiutil.c
 */
extern unsigned char BuddhaNature( char msb, char lsb );

/*
 * Some scary macros to facilitate the lookup of locations in video
 * memory
 */

#define MAKE_OFFSET( row, col ) \
        ( ( ( row ) * UIData->screen.increment ) + ( col ) )

#define ZEN_LOOKUP( typ, row, col ) \
        (& ( ( (typ)[BIOSCurPage] )[ MAKE_OFFSET( (row), (col) ) ] ) )

#ifdef __WINDOWS__
#define CODE_LOOKUP( row, col ) \
        (( LP_ZEN_CODE )&(WindowsKernelPointerTo_A000h[ MAKE_OFFSET( (row), (col) ) ]))

#define ATTR_LOOKUP( row, col ) \
        (( LP_ZEN_ATTR )&(WindowsKernelPointerTo_A200h[ MAKE_OFFSET( (row), (col) ) ]))
#else
#define CODE_LOOKUP( row, col ) \
        (ZEN_LOOKUP( code_pages, (row), (col) ))

#define ATTR_LOOKUP( row, col ) \
        (ZEN_LOOKUP( attr_pages, (row), (col) ))
#endif

/*
 * Constants that allow us to convert between AT colour attributes, which
 * for all practical purposes is the abstact representation of an attribute
 * in UI, and NEC attributes.
 */

#define IBM_BLUE                0x01
#define IBM_GREEN               0x02
#define IBM_RED                 0x04

#define NEC_BLUE                0x20
#define NEC_GREEN               0x80
#define NEC_RED                 0x40

#define NEC_REVERSE             0x04
#define NEC_BLINKING            0x02
#define NEC_UNDERLINE           0x08

