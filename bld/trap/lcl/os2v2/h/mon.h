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


#ifndef _MON_H_INCLUDED_
#define _MON_H_INCLUDED_

/* OS/2 Keyboard Monitors definitions */

#define MONITOR_DEFAULT 0x0000
#define MONITOR_BEGIN        1
#define MONITOR_END          2

typedef SHANDLE   HMONITOR;
typedef HMONITOR *PHMONITOR;

typedef struct _KEYPACKET {
  USHORT  MonFlagWord;
  UCHAR   XlatedChar;
  UCHAR   XlatedScan;
  UCHAR   DBCSStatus;
  UCHAR   DBCSShift;

  union
  {
    USHORT  ShiftState;
    USHORT  LayerIndex;
  };

  ULONG   Milliseconds;
  USHORT  KbdDDFlagWord;
} KEYPACKET;

typedef struct _MLNPACKET {
  USHORT  MonFlagWord;
  USHORT  IOCTL;
  USHORT  CPId;
  USHORT  CPIndex;
  ULONG   Reserved;
  USHORT  KbdDDFlagWord;
} MLNPACKET;

// DBCSStatus

#define SF_SHIFTS              1  // If set to 1, shift status returned without a character
#define SF_NOTCHAR             2  // 0 - Scan code is a character
                                  // 1 - Scan code is not a character;
                                  //     instead it is an extended key code from the keyboard.
#define SF_IMMEDIATE          32  // If set to 1, immediate conversion requested
#define SF_TYPEMASK          192  // Has the following values:
                                  // 00 - Undefined
                                  // 01 - Final character; interim character flag is turned off
                                  // 10 - Interim character
                                  // 11 - Final character; interim character flag is turned on.
// MonFlagWord

#define MF_OPEN                1  // open
#define MF_CLOSE               2  // close
#define MF_FLUSH               4  // is flush packet

// KbdDDFlagWord

#define KF_NOTSQPACKET      1024  // Don't put this packet in SQ buffer
#define KF_ACCENTEDKEY       512  // Key was translated using previous accent.
#define KF_MULTIMAKE         256  // Key was repeated make of a toggle key.
#define KF_SECONDARYKEY      128  // Previous scan code was the E0 prefix code.
#define KF_KEYBREAK           64  // This is the break of the key.
#define KF_KEYTYPEMASK        63  // Isolates the Key Type field of DDFlags.
#define KF_UNDEFKEY           63  // Key packet is undefined
#define KF_SYSREQKEY          23  // This key packet is the SysReq key (4990)
#define KF_PRINTFLUSHKEY      22  // This packet is Ct-Alt-PrtScr
#define KF_PSPRINTECHOKEY     21  // This packet is Ctl-P
#define KF_PRINTECHOKEY       20  // This packet is Ctl-PrtScr
#define KF_PRTSCRKEY          19  // This packet is PrtScr
#define KF_PSBREAKKEY         18  // This packet is Ctl-C
#define KF_BREAKKEY           17  // This packet is Ctl-Break
#define KF_ACCENTKEY          16  // This packet is an accent key
#define KF_XRORPNOT           13  // This packet is a Read or Peek Notification Pct.
#define KF_MLNOTIFICATION     14  // packet is a Multi-Layer NLS packet
#define KF_HOTKEYPACKET       12  // This packet is the hot key.
#define KF_BADKEYCOMBO        11  // Accent/char combo undefined, beep only.
#define KF_WAKEUPKEY          10  // This packet is one following PAUSEKEY
#define KF_PSPAUSEKEY          9  // This packet is Ctl-S
#define KF_PAUSEKEY            8  // This packet is Ctl-Numlock or PAUSE
#define KF_SHIFTMASK           7  // Key is a shift Key
#define KF_DUMPKEY             6  // This packet is Ctl-Numlock-NumLock
#define KF_REBOOTKEY           5  // This packet is Ctl-Alt-Del
#define KF_RESENDCODE          4  // This packet is resend code from controller
#define KF_OVERRUNCODE         3  // This packet is overrun code from controller
#define KF_SECPREFIXCODE       2  // This packet is E0/E1 scan code
#define KF_ACKCODE             1  // This packet is ack code from keyboard


typedef struct _MONBUF {
  USHORT    cb;
  KEYPACKET Buffer;
  BYTE      Reserved[20];
} MONBUF;

#define RS_SYSREG      32768  // Bit 15  SysReq key down
#define RS_CAPSLOCK    16384  // Bit 14  Caps Lock key down
#define RS_NUMLOCK      8192  // Bit 13  NumLock key down
#define RS_SCROLLLOCK   4096  // Bit 12  Scroll Lock key down
#define RS_RALT         2048  // Bit 11  Right Alt key down
#define RS_RCONTROL     1024  // Bit 10  Right Ctrl key down
#define RS_LALT          512  // Bit  9  Left Alt key down
#define RS_LCONTROL      256  // Bit  8  Left Ctrl key down
#define RS_INSERT        128  // Bit  7  Insert on
#define RS_CAPS           64  // Bit  6  Caps Lock on
#define RS_NUM            32  // Bit  5  NumLock on
#define RS_SCROLL         16  // Bit  4  Scroll Lock on
#define RS_ALT             8  // Bit  3  Either Alt key down
#define RS_CONTROL         4  // Bit  2  Either Ctrl key down
#define RS_LSHIFT          2  // Bit  1  Left Shift key down
#define RS_RSHIFT          1  // Bit  0  Right Shift key down


#define CS_RCONTROL     91    // Right Control
#define CS_LSHIFT       42    // Left Shift
#define CS_RSHIFT       54    // Right Shift
#define CS_LALT         56    // Left Alt
#define CS_RALT         94    // Right Alt


/* DosMon* prototypes */

#define  DosMonOpen      DOS16MONOPEN
#define  DosMonClose     DOS16MONCLOSE
#define  DosMonReg       DOS16MONREG
#define  DosMonRead      DOS16MONREAD
#define  DosMonWrite     DOS16MONWRITE
#define  DosGetInfoSeg   DOS16GETINFOSEG

APIRET16 APIENTRY16 DosMonOpen   (PSZ pszDevName, PHMONITOR phmon);
APIRET16 APIENTRY16 DosMonClose  (HMONITOR hmon);
APIRET16 APIENTRY16 DosMonReg    (HMONITOR hmon, MONBUF *pbInBuf, /*MONBUF*/ULONG *pbOutBuf, USHORT fPosition, USHORT usIndex);
APIRET16 APIENTRY16 DosMonRead   (PBYTE pbInBuf, USHORT fWait, PBYTE pbDataBuf, PUSHORT pcbData);
APIRET16 APIENTRY16 DosMonWrite  (PBYTE pbOutBuf, PBYTE pbDataBuf, USHORT cbData);

#endif /* _MON_H_INCLUDED */
