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



#define NULL 0
#include <nwtypes.h>
#include <lanconf.h>
// #include "serialIO.h"
#include <miniproc.h>
#include <string.h>
#include <loader.h>

#define InterruptSignature                              0x50544E49                      /* 'PTNI' */
#define IORegistrationSignature                 0x53524F49                      /* 'SROI' */

/* define Netware 386 OS routines */

/****************************************************************************/
#pragma aux ___GETDS =              \
      0x8C 0xD0    /* mov ax, ss */ \
      0x8E 0xD8    /* mov ds, ax */ \
      0x8E 0xC0    /* mov es, ax */ \
      modify [eax];
/****************************************************************************/
/****************************************************************************/


void Enable(void);
void Disable(void);

#define MaxPorts 2

/* defines for translate tables */
#define MaxBitRateIndex 19
#define MaxParityIndex 5
#define MaxStopBitsIndex 3
#define MaxDataBitsIndex 4


/* define offsets to comm output registers */
#define SER_BAUD_LSB 0
#define SER_BAUD_MSB 1
#define SER_TRANSMIT 0
#define SER_INT_ENABLE 1

/* define offsets for comm input/output registers */
#define SER_LINE_CTL 3
#define SER_MODEM_CTL 4

/* define offsets to comm input registers */
#define SER_RECEIVE 0
#define SER_ID 2
#define SER_LINE_STATUS 5
#define SER_MODEM_STATUS 6

/* define Bits for Interrupt Enable register */
#define SER_DATA_ENABLE 0x01
#define SER_TBRE_ENABLE 0x02
#define SER_LSR_ENABLE  0x04
#define SER_MSR_ENABLE  0x08


/* define Bits for Line Control register */
#define SEND_BREAK 0x40

/* define Bits for Modem Control register */
#define OUT2   0x08
#define RTS    0x02
#define DTR    0x01

/* define Bits for Line Status Register */
#define RECEIVE_READY 0x01
#define OVERRUN_ERROR 0x02
#define PARITY_ERROR  0x04
#define FRAMING_ERROR 0x08
#define BREAK_DETECT   0x10
#define TRANSMIT_READY 0x20


/* define Modem Status Register bits */
#define CTS      0x10
#define DSR      0x20
#define RI       0x40
#define DCD      0x80

#define NO_PENDING_INTR 0x01

#define ERROR_MASK (OVERRUN_ERROR | PARITY_ERROR | FRAMING_ERROR)

/* interrupt controller registers */
#define INT_CTRL_PORT 0x20
#define INT_MASK_PORT 0x21

/* define end of interrupt value */
#define END_OF_INT 0x20

// Return error code from SERIALIO.NLM

#define ERR_BAD_HANDLE          -1  // Bad ComPortStructured handle
#define ERR_NO_INT_AVAIL                  -2  // Com interrupts not available
#define ERR_OUT_MEMORY                    -3  // out of memory
#define ERR_INVALID_PARAMETER     -5  // invalid parameters for com ports
#define ERR_OUT_RTAG                      -4  // can't allocate resource tags
#define ERR_PORT_NOTAVAIL       -6  // requested comm ports already acquired
#define ERR_INVALID_PORT        -7  // requested comm ports not supported/installed

#define HW_XOFF_THRESHOLD       32
#define BUFFER_MIN             128
#define BUFFER_MAX           65535
#define XMT_FIFO_LENGTH            2048
#define RCV_FIFO_LENGTH       2048

// define index into Receive Error Count Array
#define OVERRUN 0
#define PARITY  1
#define FRAMING 2

#define COM1PORT 0
#define COM2PORT 1



#define ComStructSignature  0x4f495352

/* port control structure */
typedef struct ComPortStruct
   {
        struct ComPortStruct   *ComPortLink;
        WORD   ComPort;
        WORD   IrqNumber;
        LONG   ComStructID;
        WORD   IOAddress;
        BYTE   BitRate;
        BYTE   DataBits;
        BYTE   StopBits;
        BYTE   Parity;
        BYTE   FlowControl;
        BYTE   EnableMask;
        WORD   RTS_ON;
   LONG   RcvErrors[4];
        LONG   RcvErrorsSent[4];
        WORD   RTS_OFFthreshold;
        WORD   RcvBufferSize;
        WORD   XmitBufferSize;
        WORD   RcvByteCount;
        char  *ComInputPtr;
        char  *PCInputPtr;
        char  *ComOutputPtr;
        char  *PCOutputPtr;
        char  *InputBuffer;
        char  *OutputBuffer;
   struct ResourceTagStructure     *RS232IORTag;  //calling nlm's RS232 tag
   struct ResourceTagStructure     *InterruptRTag;
   struct ResourceTagStructure     *MemAllocRTag;
        }       T_ComPortStruct;



/* define Local routines */

WORD AcquireSerialPort( WORD PortNumber,
                        struct ResourceTagStructure     *RS232RTag,
                                                           int *ComPortHandle );

WORD InitSerialPort(T_ComPortStruct *ComPortHandle, BYTE UserBitRate,
                    BYTE UserDataBits, BYTE UserStopBits, BYTE UserParity,
                                                  BYTE UserFlowControl);
WORD ReleasePort(T_ComPortStruct *ComPortHandle);
WORD SetReceiveBufferSize(T_ComPortStruct *ComPortHandle, WORD BufferSize);
WORD SetTransmitBufferSize(T_ComPortStruct *ComPortHandle, WORD BufferSize);
WORD WriteData(T_ComPortStruct *ComPortHandle,char *Buffer,WORD Length);
WORD ReadData(T_ComPortStruct *ComPortHandle,char *Buffer,WORD BufferLen);
WORD ExternalStatus(T_ComPortStruct *ComPortHandle);
WORD WriteModemControlRegister(T_ComPortStruct *ComPortHandle, WORD Status);
WORD FlushReadBuffer(T_ComPortStruct *ComPortHandle);
WORD FlushWriteBuffer(T_ComPortStruct *ComPortHandle);
WORD ReadStatus(T_ComPortStruct *ComPortHandle);
LONG ReadErrors(T_ComPortStruct *ComPortHandle, LONG *Count);
WORD WriteStatus(T_ComPortStruct *ComPortHandle);
WORD SendBreak(T_ComPortStruct *ComPortHandle);
WORD ClearBreak(T_ComPortStruct *ComPortHandle);
WORD TestBreak(T_ComPortStruct *ComPortHandle);


static WORD ValidateParameters( struct ComPortStruct *p);
void _IntServiceSer1(void);
void _IntServiceSer2(void);
void TransmitChar(T_ComPortStruct *ComPortHandle);
void SerialIntProcess(T_ComPortStruct *ComPortHandle);
void StartTransmit(T_ComPortStruct *ComPortHandle);
//void interrupt _IntServiceSer1(void);
//void interrupt _IntServiceSer2(void);
WORD Peek(WORD *Pointer);
BYTE inportb(WORD PortAddress);
void outportb(WORD PortAddress, WORD Value);

extern WORD AvailableComPorts;

extern struct ComPortStruct     *ComPortListHead;

// WORD irqNumber[MaxPorts] = { 4, 3 };
void (*irqRoutine[MaxPorts])() = { _IntServiceSer1, _IntServiceSer2 };

LONG badhandle=ERR_BAD_HANDLE;

WORD BitRateTable[MaxBitRateIndex] = {  0x0900,   /*     50 Baud */
                                                                                                         0x0600,   /*     75 Baud */
                                                                                                         0x0417,   /*    110 Baud */
                                                                                                         0x0359,   /*    134.5 Baud */
                                                                                                         0x0300,   /*    150 Baud */
                                                                                                         0x0180,   /*    300 Baud */
                                                                                                         0x00C0,   /*    600 Baud */
                                                                                                         0x0060,   /*   1200 Baud */
                                                                                                         0x0040,   /*   1800 Baud */
                                                                                                         0x003A,   /*   2000 Baud */
                                                                                                         0x0030,   /*   2400 Baud */
                                                                                                         0x0020,   /*   3600 Baud */
                                                                                                         0x0018,   /*   4800 Baud */
                                                                                                         0x0010,   /*   7200 Baud */
                                                                                                         0x000C,   /*   9600 Baud */
                                                                                                         0x0006,   /*  19200 Baud */
                                                                                                         0x0003,   /*  38400 Baud */
                                                                                                         0x0002,   /*  57600 Baud */
                                                                                                         0x0001 }; /* 115200 Baud */

BYTE ParityTable[MaxParityIndex] = {    0x00,     /* no parity */
                                                                                                         0x08,     /* odd parity */
                                                                                                         0x18,     /* even parity */
                                                                                                         0x28,     /* mark parity */
                                                                                                         0x38 };   /* space parity */

BYTE StopBitTable[MaxStopBitsIndex] = { 0x00,     /* 1 stop bit */
                                                                                                         0x04,    /* 1.5 stop bits for 5 data bits */
                                                                                                         0x04 };          /* 2 stop bits */

BYTE CharacterLengthTable[MaxDataBitsIndex] = {
                                                                                                         0x00,    /* 5 bits */
                                                                                                         0x01,    /* 6 bits */
                                                                                                         0x02,    /* 7 bits */
                                                                                                         0x03};   /* 8 bits */

BYTE CharacterMaskTable[MaxDataBitsIndex] = {
                                                                                                         0x1F,    /* 5 bits */
                                                                                                         0x3F,    /* 6 bits */
                                                                                                         0x7F,    /* 7 bits */
                                                                                                         0xFF};    /* 8 bits */


/****************************************************************************/

/*
Note:
(a) The ResourceTag for the hardware interrupt vector is currently owned by the
    serdrv.nlm. This may be changed to be an input parameter passed in by the
    application later on.

(b) If this library is to be a released product, the symbols 'ReturnResourceTag'
    and 'CFindLoadModuleHandle' need to be exported by the OS.


(c) Apparently, there needs to be a delay of about 50 ms between WriteStatus
    and ReadStatus for low baudrate

*/



/****************************************************************************/


#define MAX_COM_PORTS 2

extern struct LoadDefinitionStructure *SerdrvNLMHandle;
extern struct IOConfigurationStructure ioConfig;


/*
static WORD CheckBIOS( WORD PortNumber )
   {

   if ((PortNumber < MAX_COM_PORTS) &&
                 (((Peek((WORD *)0x411) >> 1) & 0x07) > PortNumber) &&
                 ( Peek((WORD *) (0x400 + PortNumber * 2))) != 0  )
       return TRUE;
   else
            return FALSE;
   }
*/

WORD AcquireSerialPort( WORD PortNumber,
                       struct ResourceTagStructure     *RS232RTag,
                                                          int *ComPortHandle )

   {
   struct ResourceTagStructure    *InterruptRTag;
   struct ResourceTagStructure    *NLMAllocRTag;
   struct ComPortStruct *p;
        LONG rc;
        LONG eoi;
        size_t size;

        *ComPortHandle = NULL;

   if ( AvailableComPorts < PortNumber ) return ERR_INVALID_PORT;

        if ( ioConfig.CIOPortsAndRanges[PortNumber*2] == 0 )
                return ERR_PORT_NOTAVAIL;

        if ( ComPortListHead != NULL )
           {
                p = ComPortListHead;
                for (; p != NULL; p = p->ComPortLink )
                        if ( p->ComPort == PortNumber )  return ERR_PORT_NOTAVAIL;
                }

        NLMAllocRTag = AllocateResourceTag( RS232RTag->RTModule,
                                                "RS232IO Data Memory",
                                       AllocSignature );

        if ( NLMAllocRTag == NULL ) return ERR_OUT_RTAG;

        NLMAllocRTag->RTLink = RS232RTag->RTLink;
        RS232RTag->RTLink = RS232RTag->RTModule->LDResourceList;
   RS232RTag->RTModule->LDResourceList = RS232RTag;


        size = sizeof(struct ComPortStruct);
   p = (T_ComPortStruct *)Alloc( size, NLMAllocRTag );

        if ( p == NULL ) return ERR_OUT_MEMORY;

        p-> IOAddress = ioConfig.CIOPortsAndRanges[PortNumber*2];
                        /* Peek( (WORD *)(0x400 + PortNumber * 2) ); */
   p-> ComPort = PortNumber;
   p-> IrqNumber = ioConfig.CIntLine[PortNumber];

                /* take over hardware interrupt Vector */
   InterruptRTag = AllocateResourceTag( RS232RTag->RTModule,
                                       "RS232IO Interrupt Handler",
                                                InterruptSignature );

   if ( InterruptRTag == NULL ) return ERR_OUT_RTAG;

        InterruptRTag->RTLink = RS232RTag->RTLink;
        RS232RTag->RTLink = RS232RTag->RTModule->LDResourceList;
   RS232RTag->RTModule->LDResourceList = RS232RTag;

        Disable();
   rc = SetHardwareInterrupt( p->IrqNumber,
                              irqRoutine[p->ComPort],
                              InterruptRTag, 0, 0, &eoi);


        // rc=1 for invalid parms rc = 2 for interrupt not available
   // and rc = 3 for out of memory
   Enable();

        if (rc != 0)
           {
                Free(p);
                return rc;
                }

        else
           {
                p-> InputBuffer = (char *)Alloc( RCV_FIFO_LENGTH, NLMAllocRTag );
                p-> OutputBuffer = (char *)Alloc( XMT_FIFO_LENGTH, NLMAllocRTag );
                if ( p-> InputBuffer == NULL || p-> OutputBuffer == NULL )
                   {
                        Free(p);
                        return ERR_OUT_MEMORY;
                        }

                p->ComPortLink = ComPortListHead;
           ComPortListHead = p;

           p-> EnableMask = 0;
           p-> RS232IORTag = RS232RTag;
           p-> ComStructID = ComStructSignature;
                p-> MemAllocRTag = NLMAllocRTag;

                /* initialize FIFO pointers */

                p-> RcvErrors[0] = 0;
                p-> RcvErrors[1] = 0;
                p-> RcvErrors[2] = 0;
                p-> RcvErrors[3] = 0;

                p-> XmitBufferSize = XMT_FIFO_LENGTH;
      p-> ComOutputPtr = p-> OutputBuffer;
      p-> PCOutputPtr  = p-> OutputBuffer;
           p-> RcvBufferSize = RCV_FIFO_LENGTH;
      p-> ComInputPtr = p-> InputBuffer;
      p-> PCInputPtr  = p-> InputBuffer;
                p-> RcvByteCount = 0;
                p-> RTS_OFFthreshold = HW_XOFF_THRESHOLD;
                p-> FlowControl = TRUE;
                p->RS232IORTag->RTResourceCount++;
                p->InterruptRTag = InterruptRTag;
                *ComPortHandle = (int)p;
                return 0;
                }


   }

/****************************************************************************/
WORD InitSerialPort( T_ComPortStruct *ComPortHandle, BYTE UserBitRate,
                     BYTE UserDataBits, BYTE UserStopBits, BYTE UserParity,
                                                        BYTE UserFlowControl)
{
        WORD rc;
        WORD rate, id;
        BYTE controlValue;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        /* save settings in the Port Control Block */

        ComPortHandle-> BitRate =  UserBitRate;
        ComPortHandle-> DataBits = UserDataBits;
        ComPortHandle-> StopBits = UserStopBits;
        ComPortHandle-> Parity =  UserParity;

        if ((rc = ValidateParameters( ComPortHandle )) == 0)
                {
                /* if BitRate is -1 don't change callers selected rate, data bits, etc */
                if (ComPortHandle-> BitRate != -1)
                        {
                        ComPortHandle-> FlowControl =  UserFlowControl;

                        /* translate Bit Rate */
                        rate = BitRateTable[ComPortHandle-> BitRate];

                        /* translate Data Bits */
                controlValue = CharacterLengthTable[ComPortHandle-> DataBits];

                        /* translate Parity Bits */
                        controlValue |= ParityTable[ComPortHandle-> Parity];

                        /* translate Stop Bits */
                        controlValue |= StopBitTable[ComPortHandle-> StopBits];

                        /* output bit rate to the hardware */
                        outportb(ComPortHandle->IOAddress + SER_LINE_CTL, 0x80);
                        outportb(ComPortHandle->IOAddress + SER_BAUD_LSB, rate % 256);
                        outportb(ComPortHandle->IOAddress + SER_BAUD_MSB, rate / 256);

                        /* output parity, stop bits, character length to hardware */
                        outportb(ComPortHandle->IOAddress + SER_LINE_CTL, controlValue);
                        }

                /* Setting Receive Data interrupts */

                ComPortHandle-> EnableMask |= SER_DATA_ENABLE;
                outportb(ComPortHandle->IOAddress + SER_INT_ENABLE,
                         ComPortHandle-> EnableMask);
                outportb(ComPortHandle->IOAddress + SER_MODEM_CTL, OUT2 | RTS | DTR);

           ComPortHandle->RTS_ON=TRUE;
                /* Tell the interrupt controller to allow IRQ for serial port */
                outportb(INT_MASK_PORT,
                                         inportb(INT_MASK_PORT) & ~(1 << ComPortHandle->IrqNumber));

                // Make sure no outstanding pending interrupts

                id = inportb ( ComPortHandle-> IOAddress+SER_ID ) & NO_PENDING_INTR;
                if ( !id )   // there is a pending interrupt
                   {
         inportb ( ComPortHandle-> IOAddress+SER_MODEM_STATUS );
                        inportb ( ComPortHandle-> IOAddress+SER_LINE_STATUS );
                        inportb ( ComPortHandle-> IOAddress + SER_RECEIVE);
                        }
                }

        return (rc);
}
/****************************************************************************/
static WORD ValidateParameters( T_ComPortStruct *p)
{
        WORD rc;

        rc = 0;

        if ((p-> BitRate >= MaxBitRateIndex)  && (p-> BitRate != -1))
                rc = ERR_INVALID_PARAMETER;

        if (p-> DataBits >= MaxDataBitsIndex)
                rc = ERR_INVALID_PARAMETER;

        if (p-> StopBits >= MaxStopBitsIndex)
                rc = ERR_INVALID_PARAMETER;

        if (p-> Parity >= MaxParityIndex)
                rc = ERR_INVALID_PARAMETER;

        return rc;
}
/****************************************************************************/
WORD ReleasePort(T_ComPortStruct *ComPortHandle)
{
        T_ComPortStruct  *p1;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        /* Disable Interrupts from serial chip */
        Disable();
        ComPortHandle-> EnableMask = 0;
        outportb(ComPortHandle->IOAddress + SER_INT_ENABLE, ComPortHandle-> EnableMask);
        outportb(ComPortHandle->IOAddress + SER_MODEM_CTL, 0x00);

        /* Disable interrupt controller for this device */
        outportb(INT_MASK_PORT,
                                 inportb(INT_MASK_PORT) | (1 << ComPortHandle->IrqNumber));
        /* Restore Interrupt Vector */
        Disable();

        ClearHardwareInterrupt(ComPortHandle->IrqNumber,
                               irqRoutine[ComPortHandle->ComPort] );

        Enable();

        ComPortHandle->RS232IORTag->RTResourceCount--;

        // delink the current acquired port from list

        if ( ComPortHandle == ComPortListHead )
           ComPortListHead = ComPortHandle->ComPortLink;
        else
           {
                p1 = ComPortListHead;
                while (p1 != NULL)
                   {
                        if (p1->ComPortLink == ComPortHandle)
                           {
                                p1->ComPortLink = ComPortHandle->ComPortLink;
                                break;
                                }
                        else
                           p1 = p1->ComPortLink;
                        }
                }

        Free(ComPortHandle->InputBuffer);
        Free(ComPortHandle->OutputBuffer);
        Free(ComPortHandle);


        return 0;
}

/***************************************************************************

   SetReceiveBufferSize(T_ComPortStruct *ComPortHandle, WORD BufferSize)

      Allocate a buffer of size BufferSize and copy the buffer pointer
      into the structure PORT for COM port given by PortNumber. This
      function will return an error code if it is called before
      AcquireSerialPort(). The function will return the size of buffer
      that has actually been allocated which may be different from
      BufferSize.

***************************************************************************/

WORD SetReceiveBufferSize(T_ComPortStruct *ComPortHandle, WORD BufferSize)
   {
        char *tmp;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        if ( BufferSize < BUFFER_MIN )  BufferSize = BUFFER_MIN;
        if ( BufferSize > BUFFER_MAX )  BufferSize = BUFFER_MAX;

        tmp = (char *)Alloc( BufferSize, ComPortHandle->MemAllocRTag );

        if ( tmp != NULL )              // allocate ok, use new buffer
                {
        Free(ComPortHandle->InputBuffer);
           ComPortHandle->InputBuffer = tmp;
                }

        else                                                    // cannot allocate, use old buffer
           {
                if  ( BufferSize > ComPortHandle->RcvBufferSize )
                    BufferSize = ComPortHandle->RcvBufferSize;
                            // use old buffersize
                }

        ComPortHandle-> RcvBufferSize = BufferSize;
   ComPortHandle-> ComInputPtr = ComPortHandle-> InputBuffer;
   ComPortHandle-> PCInputPtr = ComPortHandle-> InputBuffer;
        ComPortHandle-> RcvByteCount = 0;

        return BufferSize;
        }

WORD SetTransmitBufferSize(T_ComPortStruct *ComPortHandle, WORD BufferSize)
   {
        char *tmp;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        if ( BufferSize < BUFFER_MIN )  BufferSize = BUFFER_MIN;
        if ( BufferSize > BUFFER_MAX )  BufferSize = BUFFER_MAX;

        tmp = (char *)Alloc( BufferSize, ComPortHandle->MemAllocRTag );

        if ( tmp != NULL )              // allocate ok, use new buffer
                {
        Free(ComPortHandle->OutputBuffer);
           ComPortHandle->OutputBuffer = tmp;
                }

        else                                                    // cannot allocate, use old buffer
           {
                if  ( BufferSize > ComPortHandle->XmitBufferSize )
                    BufferSize = ComPortHandle->XmitBufferSize;
                            // use old buffersize
                }

        ComPortHandle-> XmitBufferSize = BufferSize;
   ComPortHandle-> ComOutputPtr = ComPortHandle-> OutputBuffer;
   ComPortHandle-> PCOutputPtr = ComPortHandle-> OutputBuffer;

        return BufferSize;
        }


/****************************************************************************/
/* returns Number of characters written                                                                                                  */
/****************************************************************************/
WORD WriteData(T_ComPortStruct *ComPortHandle, char *Buffer, WORD Length)
{
        char *NewPCOutput;
        WORD i;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        i = 0;

        for ( ; i < Length; i++)
                {
                NewPCOutput = ComPortHandle-> PCOutputPtr + 1;
                   /* Make pointer circular */
           if ( NewPCOutput == (ComPortHandle-> OutputBuffer) +
                                ComPortHandle->XmitBufferSize )
                NewPCOutput = ComPortHandle-> OutputBuffer;

                if (NewPCOutput == ComPortHandle-> ComOutputPtr)
                   break;
                else
                        {
                        ComPortHandle-> PCOutputPtr[0] = Buffer[i];
                        ComPortHandle-> PCOutputPtr = NewPCOutput;
                        }
           }
        StartTransmit(ComPortHandle);

        return i;
}

/*********)******************************************************************/
/* returns Number of read                                                                                                                                        */
/****************************************************************************/
WORD ReadData(T_ComPortStruct *ComPortHandle, char *Buffer, WORD Length)
{
        char *NewPCInput;
        WORD i;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        i = 0;

        Disable();
        if ( ComPortHandle->RcvByteCount == 0 ) return i;
        Enable();

        while( ComPortHandle-> PCInputPtr != ComPortHandle->ComInputPtr &&
               i < Length && ComPortHandle->RcvByteCount > 0)
                {
                Buffer[i] = ComPortHandle-> PCInputPtr[0];
                i++;
      ComPortHandle->RcvByteCount--;

                /* Update circular buffer Pointer */
                NewPCInput = ComPortHandle-> PCInputPtr + 1;
           if ( NewPCInput == (ComPortHandle-> InputBuffer) +
                                       ComPortHandle->RcvBufferSize )
                        NewPCInput = ComPortHandle-> InputBuffer;
                ComPortHandle-> PCInputPtr = NewPCInput;
           if (ComPortHandle->FlowControl && ComPortHandle->RTS_ON == FALSE)
                       // Handware Handshake and RTS was off
                   {
                   if ( ComPortHandle->RcvBufferSize - ComPortHandle->RcvByteCount >
                        ComPortHandle->RTS_OFFthreshold )   // there is more room
                                {
                                WriteModemControlRegister ( ComPortHandle,
                                  inportb(ComPortHandle->IOAddress+SER_MODEM_CTL) | RTS );
                                  // Turn RTS on
                                ComPortHandle->RTS_ON=TRUE;

                                }
                   }

                }
        return i;
}
/****************************************************************************/
WORD ExternalStatus(T_ComPortStruct *ComPortHandle)
{
        WORD Status;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        Status = inportb(ComPortHandle->IOAddress+SER_MODEM_STATUS);
        return Status & 0x7fff;
}
/****************************************************************************/
WORD WriteModemControlRegister(T_ComPortStruct *ComPortHandle, WORD Status)
   {

// Only writes to DTR and RTS lines

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        Status = ( Status & (RTS | DTR) ) |
                 ( inportb(ComPortHandle->IOAddress+SER_MODEM_CTL) & ~(DTR | RTS) ) ;

        outportb(ComPortHandle->IOAddress + SER_MODEM_CTL, Status );

        return Status;  // return the actual modem control word output

   }
/****************************************************************************/
WORD FlushReadBuffer(T_ComPortStruct *ComPortHandle)
{

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        Disable();
                /* reset FIFO pointers */
        ComPortHandle-> ComInputPtr = ComPortHandle-> InputBuffer;
        ComPortHandle-> PCInputPtr = ComPortHandle-> InputBuffer;

                /* clear out the COM port */
        inportb(ComPortHandle-> IOAddress + SER_RECEIVE);
        Enable();

        return 0;
}
/****************************************************************************/
WORD FlushWriteBuffer(T_ComPortStruct *ComPortHandle)
{

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        Disable();
        /* reset FIFO pointers */
        ComPortHandle-> ComOutputPtr = ComPortHandle-> OutputBuffer;
        ComPortHandle-> PCOutputPtr = ComPortHandle-> OutputBuffer;
        Enable();

        return 0;
}
/****************************************************************************/
WORD ReadStatus(T_ComPortStruct *ComPortHandle)
   {

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        return ComPortHandle->RcvByteCount;
        }
/****************************************************************************/
LONG ReadErrors(T_ComPortStruct *ComPortHandle, LONG *Count)
// return a pointer to a LONG array with 3 elements :
//   *ptr   = # of OverRun errors accumulated
//   *ptr+1 = # of Parity errors accumulated
//   *ptr+2 = # of Framing errors accumulated
//   *ptr+3 = # of bytes for missed characters
//      all the error counts will be reset to zeroes after the read
   {
        LONG total=0;
        WORD i;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        CMovD( (LONG *)&(ComPortHandle->RcvErrors[0]),
               (LONG *)&(ComPortHandle->RcvErrorsSent[0]),
                         4 );
/*      ComPortHandle-> RcvErrorsSent[0] = ComPortHandle-> RcvErrors[0];
        ComPortHandle-> RcvErrorsSent[1] = ComPortHandle-> RcvErrors[1];
        ComPortHandle-> RcvErrorsSent[2] = ComPortHandle-> RcvErrors[2];
        ComPortHandle-> RcvErrorsSent[3] = ComPortHandle-> RcvErrors[3];
*/
        for (i=0; i<4; i++)
            total += ComPortHandle->RcvErrors[i];

   ComPortHandle-> RcvErrors[0] = 0;
        ComPortHandle-> RcvErrors[1] = 0;
        ComPortHandle-> RcvErrors[2] = 0;
        ComPortHandle-> RcvErrors[3] = 0;

        Count = (LONG *)&(ComPortHandle->RcvErrorsSent[0]);
        return total;
   }

/****************************************************************************/
WORD WriteStatus(T_ComPortStruct *ComPortHandle)
{
        WORD WriteComplete;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        WriteComplete = FALSE;
        if (ComPortHandle-> PCOutputPtr == ComPortHandle-> ComOutputPtr)
                WriteComplete = TRUE;
        return(WriteComplete);
}

/****************************************************************************/
WORD SendBreak(T_ComPortStruct *ComPortHandle)
{

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        /* Dont try to send data during Break */
        FlushWriteBuffer(ComPortHandle);
        Disable();
        outportb(ComPortHandle->IOAddress + SER_LINE_CTL,
                         (inportb(ComPortHandle->IOAddress + SER_LINE_CTL) | SEND_BREAK));
        Enable();

        return 0;
}

/****************************************************************************/
WORD ClearBreak(T_ComPortStruct *ComPortHandle)
{

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        /* Dont try to send data during Break */
        Disable();
        outportb( ComPortHandle->IOAddress + SER_LINE_CTL,
                            (inportb(ComPortHandle->IOAddress + SER_LINE_CTL) & ~SEND_BREAK));
   Enable();
        return 0;
}
/****************************************************************************/
WORD TestBreak(T_ComPortStruct *ComPortHandle)
{
        WORD BreakState;

        if ( ComPortListHead == NULL ||
             ComPortHandle->ComStructID != ComStructSignature )
      return ERR_BAD_HANDLE;

        BreakState = 0;
        /* Dont try to send data during Break */
        Disable();
        if (inportb(ComPortHandle->IOAddress + SER_LINE_STATUS) & BREAK_DETECT)
                BreakState++;
        Enable();

        return BreakState;
}

/****************************************************************************/
//void interrupt _IntServiceSer1(void)
void _IntServiceSer1(void)   // interrupt for COM1 port
{
        T_ComPortStruct *ComPortHandle;


        if ( ComPortListHead != NULL )
           {
           ComPortHandle = ComPortListHead;
           for (; ComPortHandle != NULL; ComPortHandle = ComPortHandle->ComPortLink )
                   if ( ComPortHandle->ComPort == COM1PORT )  break;
           if ( ComPortHandle != NULL )
                   SerialIntProcess(ComPortHandle);
           }
        outportb(INT_CTRL_PORT, END_OF_INT);
}
/****************************************************************************/
//void interrupt _IntServiceSer2(void)
void _IntServiceSer2(void)   // interrupt for COM2 port
{
        T_ComPortStruct *ComPortHandle;


        if ( ComPortListHead != NULL )
           {
           ComPortHandle = ComPortListHead;
           for (; ComPortHandle != NULL; ComPortHandle = ComPortHandle->ComPortLink )
                   if ( ComPortHandle->ComPort == COM2PORT )  break;
           if ( ComPortHandle != NULL )
                   SerialIntProcess(ComPortHandle);
           }
        outportb(INT_CTRL_PORT, END_OF_INT);
}

/****************************************************************************/
void SerialIntProcess(T_ComPortStruct *ComPortHandle)
{
        BYTE inByte, bytenotread;
        char *NewInputPtr;
        WORD error_mask;

        error_mask = inportb(ComPortHandle-> IOAddress + SER_LINE_STATUS);


        if ( error_mask & OVERRUN_ERROR )
           ComPortHandle-> RcvErrors[0]++;
        if ( error_mask & PARITY_ERROR )
           ComPortHandle-> RcvErrors[1]++;
        if ( error_mask & FRAMING_ERROR )
           ComPortHandle-> RcvErrors[2]++;

//   if ((inportb(ComPortHandle-> IOAddress + SER_LINE_STATUS)) & RECEIVE_READY)
   if ( error_mask & RECEIVE_READY)
                {
                inByte = inportb(ComPortHandle-> IOAddress + SER_RECEIVE);
                inByte &= CharacterMaskTable[ComPortHandle-> DataBits];
                bytenotread = 1;

                /* check if room in the FIFO */
                if ( ComPortHandle->FlowControl &&
                   ( ComPortHandle->RcvBufferSize - ComPortHandle->RcvByteCount <=
                        ComPortHandle->RTS_OFFthreshold ) ) // there is no room with HW
                                                              // handshaking
                        {

                        if ( ComPortHandle->RTS_ON == TRUE )                    // should send xoff and RTS is on
                           {
                           WriteModemControlRegister ( ComPortHandle,
                              inportb(ComPortHandle->IOAddress+SER_MODEM_CTL) & ~RTS );
                              // Turn RTS off
                           ComPortHandle->RTS_ON = FALSE;
                           }
                           // RTS already off
                        }

                // No flowcontrol or there is room
           /* update FIFO pointer */
      NewInputPtr = ComPortHandle-> ComInputPtr + 1;
           if (NewInputPtr == ComPortHandle-> InputBuffer + ComPortHandle->RcvBufferSize )
           NewInputPtr = ComPortHandle-> InputBuffer;
           if (NewInputPtr != ComPortHandle->PCInputPtr)
                   {
                   ComPortHandle-> ComInputPtr[0] = inByte;
                   ComPortHandle-> ComInputPtr = NewInputPtr;
                   bytenotread = 0;
                   ComPortHandle-> RcvByteCount++;
                   }
           ComPortHandle->RcvErrors[3] += bytenotread;
                }

        /* try to transmit character if its ready */
        TransmitChar(ComPortHandle);
        return;
}

/****************************************************************************/
void StartTransmit(T_ComPortStruct *ComPortHandle)
{

        Disable();
        /* if transmitter is not running start it */
   if ((ComPortHandle-> EnableMask & (SER_TBRE_ENABLE | SER_MSR_ENABLE)) == 0)
//   if ((ComPortHandle-> EnableMask & SER_TBRE_ENABLE ) == 0)
      {
    /* enable transmit buffer empty interrupts */
      ComPortHandle-> EnableMask |= SER_TBRE_ENABLE;
      outportb(ComPortHandle->IOAddress + SER_INT_ENABLE,
                         ComPortHandle-> EnableMask);
      }

   Enable();
        return;
}
/****************************************************************************/
extern struct ScreenStruct *sID;

void TransmitChar(T_ComPortStruct *ComPortHandle)
{
        char *NewOutput;
        BYTE CurrentMask;
        WORD msr;

        CurrentMask = ComPortHandle-> EnableMask;
   msr = inportb ( ComPortHandle-> IOAddress+SER_MODEM_STATUS ) & (CTS|DSR);

        if ( ( ComPortHandle-> FlowControl == 0 ) || msr == (CTS|DSR) )
                {

           /* If data is buffered lets try to send it */
           if (ComPortHandle-> ComOutputPtr != ComPortHandle-> PCOutputPtr)
                   {
                        /* make sure hardware can take character */
                        if (inportb(ComPortHandle->IOAddress + SER_LINE_STATUS) &
                                                                             TRANSMIT_READY)
                                {
                                outportb(ComPortHandle-> IOAddress + SER_TRANSMIT,
                                         ComPortHandle-> ComOutputPtr[0]);
                                NewOutput = ComPortHandle-> ComOutputPtr + 1;
                           if (NewOutput == ComPortHandle-> OutputBuffer +
                                                    ComPortHandle->XmitBufferSize )
                                        NewOutput = ComPortHandle-> OutputBuffer;
                                ComPortHandle-> ComOutputPtr = NewOutput;
                                }

                        /* Send next character when Transmit buffer is empty */
                        ComPortHandle-> EnableMask &= ~SER_MSR_ENABLE;
                        ComPortHandle-> EnableMask |= SER_TBRE_ENABLE;
                        }
        else
                {
                   /* no more characters in FIFO, so turn off all interrupts */
                ComPortHandle-> EnableMask &= ~(SER_TBRE_ENABLE | SER_MSR_ENABLE);
                   ComPortHandle-> EnableMask &= ~SER_TBRE_ENABLE;
                   }
        }
        else
                {
                /* cannot send due to hardware flow control so wait for status change*/
                ComPortHandle-> EnableMask |= SER_MSR_ENABLE;
                ComPortHandle-> EnableMask &= ~SER_TBRE_ENABLE;
                }

        /* if mask changed output mask to hardware */
        if (CurrentMask != ComPortHandle-> EnableMask)
                outportb(ComPortHandle->IOAddress + SER_INT_ENABLE,
                         ComPortHandle-> EnableMask);

        return;
}

/****************************************************************************/
WORD Peek(pointer)
WORD *pointer;
{
return(((WORD *)MapAbsoluteAddressToDataOffset((LONG) pointer))[0]);
}

/****************************************************************************/
#pragma aux inportb =                             \
         0xEC                   /* in  al, dx */    \
         parm  [dx]                                                       \
         value [al];

/****************************************************************************/
#pragma aux outportb =                            \
        0xEE               /* out dx, al */       \
         parm  [dx]  [al];

