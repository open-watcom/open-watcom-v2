.func _bios_serialcom
#include <bios.h>
unsigned short _bios_serialcom( unsigned service,
                                unsigned serial_port,
                                unsigned data );
.ixfunc2 '&BiosFunc' &func
.funcend
.desc begin
The &func function uses INT 0x14 to provide serial communications
services to the serial port specified by
.arg serial_port
.ct .li .
0 represents COM1, 1 represents COM2, etc.
The values for service are:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _COM_INIT
Initializes the serial port to the parameters specified in
.arg data
.ct .li .
.term _COM_SEND
Transmits the low-order byte of
.arg data
to the serial port.
.term _COM_RECEIVE
Reads an input character from the serial port.
.term _COM_STATUS
Returns the current status of the serial port.
.endterm
.pc
The value passed in
.arg data
for the
.kw _COM_INIT
service can be built using the appropriate combination of the
following values:
.begnote $compact $setptnt 21
.termhd1 Value
.termhd2 Meaning
.note _COM_110
110 baud
.note _COM_150
150 baud
.note _COM_300
300 baud
.note _COM_600
600 baud
.note _COM_1200
1200 baud
.note _COM_2400
2400 baud
.note _COM_4800
4800 baud
.note _COM_9600
9600 baud
.sk 1
.note _COM_NOPARITY
No parity
.note _COM_EVENPARITY
Even parity
.note _COM_ODDPARITY
Odd parity
.sk 1
.note _COM_CHR7
7 data bits
.note _COM_CHR8
8 data bits
.sk 1
.note _COM_STOP1
1 stop bit
.note _COM_STOP2
2 stop bits
.endnote
.desc end
.return begin
The &func function returns a 16-bit value with the high-order byte
containing status information defined as follows:
.begnote $compact $setptnt 12
.termhd1 Bit
.termhd2 Meaning
.note bit 15 (0x8000)
Timed out
.note bit 14 (0x4000)
Transmit shift register empty
.note bit 13 (0x2000)
Transmit holding register empty
.note bit 12 (0x1000)
Break detected
.note bit 11 (0x0800)
Framing error
.note bit 10 (0x0400)
Parity error
.note bit 9  (0x0200)
Overrun error
.note bit 8  (0x0100)
Data ready
.endnote
.pp
The low-order byte of the return value depends on the value of the
.arg service
argument.
.pp
When
.arg service
is _COM_SEND, bit 15 will be set if the
.arg data
could not be sent.
If bit 15 is clear, the return value equals the byte sent.
.pp
When
.arg service
is _COM_RECEIVE, the byte read will be returned in the low-order byte
if there was no error.
If there was an error, at least one of the high-order status bits
will be set.
.pp
When
.arg service
is _COM_INIT or _COM_STATUS the low-order bits are defined as follows:
.begnote $compact $setptnt 12
.termhd1 Bit
.termhd2 Meaning
.note bit 0 (0x01)
Clear to send (CTS) changed
.note bit 1 (0x02)
Data set ready changed
.note bit 2 (0x04)
Trailing-edge ring detector
.note bit 3 (0x08)
Receive line signal detector changed
.note bit 4 (0x10)
Clear to send
.note bit 5 (0x20)
Data-set ready
.note bit 6 (0x40)
Ring indicator
.note bit 7 (0x80)
Receive-line signal detected
.endnote
.return end
.cp 15
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short status;
.exmp break
    status = _bios_serialcom( _COM_STATUS, 1, 0 );
    printf( "Serial status: 0x%2.2X\n", status );
  }
.exmp end
.class BIOS
.system
