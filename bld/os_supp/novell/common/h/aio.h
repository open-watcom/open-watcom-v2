#ifndef _AIO_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _AIO_H_F39997A6_88FC_434B_B339_554BE343B3E8

/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*    Portions Copyright (c) 1989-2002 Novell, Inc.  All Rights Reserved.                      
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
*   This header file was generated for the OpenWatcom project by Carl Young
*       carl.young@keycomm.co.uk
*   Any problems or updates required, please either contact the author or
*   the OpenWatcom contributors forums. 
*       http://www.openwatcom.com/
*
* Description:  Defines all we require to support serial functionality on
*               NetWare.
*
****************************************************************************/

#include <ownwsupp.h>

#define AIO_COMX_TYPE             (1)

#define AIO_SUCCESS               (0)
#define AIO_BAD_HANDLE            (-1)
#define AIO_FAILURE               (-2)
#define AIO_FUNC_NOT_SUPPORTED    (-3)
#define AIO_INVALID_PARAMETER     (-5)
#define AIO_PORT_NOT_AVAILABLE    (-6)
#define AIO_QUALIFIED_SUCCESS     (-7)
#define AIO_NO_MORE_PORTS         (-8)
#define AIO_TYPE_NUMBER_INVALID   (-10)
#define AIO_BOARD_NUMBER_INVALID  (-11)
#define AIO_PORT_NUMBER_INVALID   (-12)
#define AIO_RESOURCE_TAG_INVALID  (-13)
#define AIO_DATA_PRESENT          (-14)
#define AIO_BAD_REQUEST_TYPE      (-15)
#define AIO_PORT_GONE             (-20)
#define AIO_RTAG_INVALID          (-21)

#define AIO_FLUSH_WRITE_BUFFER    (0x0001)
#define AIO_FLUSH_READ_BUFFER     (0x0002)

#define AIO_FLOW_CONTROL          (3)
#define AIO_SOFTWARE_FLOW_CONTROL_OFF  (0)
#define AIO_SOFTWARE_FLOW_CONTROL_ON   (1)
#define AIO_HARDWARE_FLOW_CONTROL_OFF  (0)
#define AIO_HARDWARE_FLOW_CONTROL_ON   (2)

#define AIO_HARDWARE_TYPE_WILDCARD  (-1)
#define AIO_BOARD_NUMBER_WILDCARD   (-1)
#define AIO_PORT_NUMBER_WILDCARD    (-1)
                          
#define AIO_BAUD_50       (0)
#define AIO_BAUD_75       (1)
#define AIO_BAUD_110      (2)
#define AIO_BAUD_134p5    (3)
#define AIO_BAUD_150      (4)
#define AIO_BAUD_300      (5)
#define AIO_BAUD_600      (6)
#define AIO_BAUD_1200     (7)
#define AIO_BAUD_1800     (8)
#define AIO_BAUD_2000     (9)
#define AIO_BAUD_2400     (10)
#define AIO_BAUD_3600     (11)
#define AIO_BAUD_4800     (12)
#define AIO_BAUD_7200     (13)
#define AIO_BAUD_9600     (14)
#define AIO_BAUD_19200    (15)
#define AIO_BAUD_38400    (16)
#define AIO_BAUD_57600    (17)
#define AIO_BAUD_115200   (18)
                          
/* DataBits */

#define AIO_DATA_BITS_5   (0)
#define AIO_DATA_BITS_6   (1)
#define AIO_DATA_BITS_7   (2)
#define AIO_DATA_BITS_8   (3)

/* StopBits */
                             
#define AIO_STOP_BITS_1   (0)
#define AIO_STOP_BITS_1p5 (1)
#define AIO_STOP_BITS_2   (2)
                            
/* Parity */

#define AIO_PARITY_NONE   (0)
#define AIO_PARITY_ODD    (1)
#define AIO_PARITY_EVEN   (2)
#define AIO_PARITY_MARK   (3)
#define AIO_PARITY_SPACE  (4)

/* FlowControl */
                                      
#define AIO_SOFTWARE_FLOWCONTROL_OFF  (0)
#define AIO_SOFTWARE_FLOWCONTROL_ON   (1)
#define AIO_HARDWARE_FLOWCONTROL_OFF  (0)
#define AIO_HARDWARE_FLOWCONTROL_ON   (2)

#define AIO_DROPOUT_VALUE   (0xFF)

CPP_START

extern  int  AIOWriteStatus           ( int   portHandle,
                                        LONG  *count,
                                        WORD  *state
                                      );

extern  int  AIOFlushBuffers          ( int   portHandle,
                                        WORD  flushFlag
                                      );

extern  int  AIOWriteData             ( int   portHandle,
                                        char  *buffer,
                                        LONG  length,
                                        LONG  *numberBytesWritten
                                      );

extern  int  AIOAcquirePortWithRTag   ( int   *hardwareType,
                                        int   *boardNumber,       
                                        int   *portNumber,        
                                        int   *portHandle,        
                                        LONG   RTag
                                      );

extern  int  AIOReadData              ( int   portHandle,
                                        char  *buffer,
                                        LONG  length,
                                        LONG  *numberBytesRead
                                      );
        
extern  int  AIOReadStatus            ( int   portHandle, 
                                        LONG  *count,
                                        WORD  *state
                                      );

extern  int  AIOConfigurePort         ( int   portHandle,
                                        BYTE  bitRate,
                                        BYTE  dataBits,
                                        BYTE  stopBits,
                                        BYTE  parityMode,
                                        BYTE  flowCtrlMode
                                      );

extern  int  AIOReleasePort           ( int   portHandle );

CPP_END

#endif	/* _AIO_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
