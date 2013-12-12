.func _bios_serialcom for NEC PC98
.synop begin
#include <bios.h>
unsigned short _bios_serialcom( unsigned service,
                                unsigned port,
                                struct com_t *serialcom );
struct com_t
{
  unsigned baud;     /* Baud rate                      */
  unsigned mode;     /* Mode set                       */
  unsigned command;  /* Command specified              */
  unsigned tx_time;  /* Period of time-out for send    */
  unsigned rx_time;  /* Period of time-out for receive */
  unsigned size;     /* Buffer size to receive         */
  void _far *buffer; /* Pointer to receive buffer      */
};
.ixfunc2 '&BiosFunc' &func
.synop end
.desc begin
The
.id &func.
function provides serial communications services to the
serial port specified by
.arg port
.ct .li .
The values for
.arg port
may be:
.begterm
.term _COM_CH1
serial port 1
.term _COM_CH2
serial port 2
.term _COM_CH3
serial port 3
.endterm
.np
The values for service are:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _COM_COMMAND
It outputs the command information. It uses the
.mono command
field of the
structure
.kw com_t
only.
.term _COM_GETDTL
It gets the data length of the valid data in the receive buffer. The
received data length is stored in the
.mono size
field of the structure
.kw com_t
.ct .li .
.term _COM_INIT
It initializes the serial port specified in the argument
.arg port
.ct .li .
It uses all fields of the structure
.kw com_t
.ct .li .
When _COM_CH2 or _COM_CH3 is specified in the argument
.arg port
.ct , the
.mono baud
field of the structure
.kw com_t
is ignored.
.term _COM_INITX
It is the same function as _COM_INIT except it can set the X parameter
(flow control).
.term _COM_RECEIVE
It receives data.
It stores the received data in the field
.mono buffer+0,
the status in the field
.mono buffer+1
if no error occurred, and so on.
.term _COM_SEND
It sends one byte of data in the field
.mono buffer
of the structure
.kw com_t
.term _COM_STATUS
It gets the status of the controller and the system port. It stores
the information of the controller information in the field
.mono buffer+0
and the system port information in the field
.mono buffer+1
of the structure
.kw com_t
.ct .li .
.np
The returned controller information is described below.
.desc end
.return begin
The
.id &func.
function returns zero if no error occurred, non-zero if an
error occurred.
When the
.kw _COM_STATUS
service is requested the controller information returned is as
follows:
.begnote $compact $setptnt 12
.termhd1 Bit
.termhd2 Meaning
.note bit 0
Send status (1=Ready / 0=Busy)
.note bit 1
Receive status (1=Ready / 0=Busy)
.note bit 2
Send buffer (1=Empty / 0=Full)
.note bit 3
Parity error (1=Error / 0=No error)
.note bit 4
Overrun error (1=Error / 0=No error)
.note bit 5
Framing error (1=Error / 0=No error)
.note bit 6
Detect break status (1=Detect / 0=Not detect)
.note bit 7
Data set ready (1=On / 0=Off)
.endnote
.np
When the
.kw _COM_STATUS
service is requested the system port information is as follows:
.begnote $compact $setptnt 12
.termhd1 Bit
.termhd2 Meaning
.note bits 0-4
Not used
.note bit 5
Received carrier detect  1=Not detect / 0=Detect
.note bit 6
Send 1=Disable / 0=Enable
.note bit 7
Receive 1=No / 0=Yes
.endnote
.return end
.exmp begin
#include <bios.h>
#include <stdio.h>

void main()
  {
    unsigned  status;

    status = _bios_serialcom( _COM_STATUS, _COM_CH1, 0 );

    /*
     * Report status of serial port and test whether there
     * is a responding device (such as a modem) for each.
     * If data-set-ready and clear-to-send bits are set,
     * a device is responding.
     */
    printf( "COM status: %.4X\tActive: %s\n",
            status, ( status & 0x0003 ) ? "YES" : "NO" );
  }
.exmp end
.class BIOS
.system
