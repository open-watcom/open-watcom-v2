.chap *refid=vidwire Wiring For Remote Debugging
.*
This appendix describes both serial and parallel port cable wiring for
remote debugging.
.*
.section *refid=vidwirs Serial Port Wiring Considerations
.*
.np
.ix 'remote debugging' 'serial port wiring'
.ix 'wiring' 'serial port'
.ix 'serial port wiring'
If you plan to use the serial port Debug Server "SERSERV", a cable
must connect the serial ports of the two computer systems.
The following diagram illustrates the wiring between the two serial
ports.
If your computer systems have more than one serial port, any serial
port may be used.
.cp 24
.code begin
    Task Machine          Debugger Machine
      Serial                  Serial
     Connector               Connector
.sk
     Pin #              Pin #
         1 (PG) <---------->1 (PG)
.sk
         2 (TxD)<---------->3 (RxD)
.sk
         3 (RxD)<---------->2 (TxD)
.sk
 ------- 4 (RTS)            4 (RTS) -------
|                                          |
 ------> 5 (CTS)            5 (CTS) <------
.sk
 ------> 6 (DSR)            6 (DSR) <------
|                                          |
|        7 (SG) <---------->7 (SG)         |
|                                          |
|------> 8 (DCD)            8 (DCD) <------|
|                                          |
 ------ 20 (DTR)           20 (DTR) -------
.code end
:FIG.
:FIGCAP.Serial Port Wiring Scheme
:eFIG.
.np
Note that the wiring is symmetrical (i.e., either end of the
cable can be plugged into either PC).
.ix 'wiring' 'null modem'
.ix 'null modem wiring'
This particular arrangement of the wiring is sometimes called
a "null modem" (since pins 2 and 3 are crossed and no modem is
involved).
.*
.section *refid=vidwirp Parallel Port Wiring Considerations
.*
.np
.ix 'remote debugging' 'parallel port wiring'
.ix 'wiring' 'parallel port'
.ix 'parallel port' 'wiring'
If you plan to use the parallel port Debug Server "PARSERV" or
"PARSERVW", a cable must connect the parallel ports of the two
computer systems.
.ix 'LapLink cable'
.ix 'cable' 'LapLink'
.ix 'Travelling Software'
.ix 'Flying Dutchman cable'
.ix 'cable' 'Flying Dutchman'
.ix 'Cyco'
.ix 'cable' 'Watcom's own'
Three cabling methods are supported - the LapLink cable, the Flying
Dutchman cable, and Watcom's own design.
There are two advantages to using the LapLink or Flying Dutchman
cable:
.autonote
.note
They are commercially available (you may already own one).
.note
They may work with more PC "compatibles" than Watcom's cable.
Watcom's cabling requires 8 bi-directional data lines in the parallel
port and some PC "compatibles" do not support this.
.endnote
.np
The disadvantage with the LapLink and Flying Dutchman cables is that
they are slower than Watcom's cable since only 4 bits are transmitted
in parallel versus 8 bits for Watcom's.
Thus Watcom's cable is faster but it will have to be custom made.
.np
The LapLink cable is available from:
.illust begin
Travelling Software, Inc.
18702 North Creek Parkway
Bothell, Washington,
U.S.A. 98011
Telephone: (206) 483-8088
.illust end
.np
The Flying Dutchman cable is available from:
.illust begin
Cyco,
Adm. Banckertweg 2a,
2315 SR Leiden,
The Netherlands.
.illust end
.np
The following diagram illustrates Watcom's cable wiring between the
two parallel ports.
.cp 25
.code begin
   Task Machine       Debugger Machine
Parallel Connector   Parallel Connector

     Pin Number         Pin Number
         1 <--------------> 2
         2 <--------------> 1
         3 <--------------> 14
         4 <--------------> 16
         5 <--------------> 15
         6 <--------------> 13
         7 <--------------> 12
         8 <--------------> 10
         9 <--------------> 11
        10 <--------------> 8
        11 <--------------> 9
        12 <--------------> 7
        13 <--------------> 6
        14 <--------------> 3
        15 <--------------> 5
        16 <--------------> 4
        17 <--------------> 17
        18 <--------------> 18
.code end
:FIG.
:FIGCAP.Watcom Cable Wiring Scheme
:eFIG.
.np
The following diagram illustrates the LapLink cable wiring between the
two parallel ports.
.cp 15
.code begin
   Task Machine       Debugger Machine
Parallel Connector   Parallel Connector

     Pin Number         Pin Number
         2  --------------> 15
         3  --------------> 13
         4  --------------> 12
         5  --------------> 10
         6  --------------> 11
        10 <--------------   5
        11 <--------------   6
        12 <--------------   4
        13 <--------------   3
        15 <--------------   2
        25 <--------------> 25
.code end
:FIG.
:FIGCAP.LapLink Cable Wiring Scheme
:eFIG.
.np
The following diagram illustrates the Flying Dutchman cable wiring
between the two parallel ports.
.cp 15
.code begin
   Task Machine       Debugger Machine
Parallel Connector   Parallel Connector

     Pin Number         Pin Number
         1  --------------> 11
         2  --------------> 15
         3  --------------> 13
         4  --------------> 12
         5  --------------> 10
        10 <--------------   5
        11 <--------------   1
        12 <--------------   4
        13 <--------------   3
        15 <--------------   2
.code end
:FIG.
:FIGCAP.Flying Dutchman Cable Wiring Scheme
:eFIG.
.np
For the IBM PC and PS/2, the connectors are standard "male" DB-25
connectors.
Note that, in all cases, the wiring is symmetrical (i.e., either end
of the cable can be plugged into either PC).
.remark
Although the wiring is different for all three cables, the &company
parallel communications software can determine which one is in use.
.eremark
