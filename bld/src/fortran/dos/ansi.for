
* ANSI Control Sequences Demo Program

* To make use of IBM PC DOS extended screen and keyboard control,
* you must place the following line in the CONFIG.SYS file on
* your boot disk.

*      DEVICE=[d:][path]ANSI.SYS

* Example:      DEVICE=C:\DOS\ANSI.SYS

* IBM PC DOS supports the following terminal control sequences:

*       ESC [ # ; # H   Position to row;column. Default 1;1 (home).
*       ESC [ # A       Move the cursor up # rows. Default #=1.
*       ESC [ # B       Move the cursor down # rows. Default #=1.
*       ESC [ # C       Move the cursor forward # columns. Default #=1.
*       ESC [ # D       Move the cursor back # columns. Default #=1.
*       ESC [ # ; # f   Move the cursor to line;column. Default 1;1 (home).
*       ESC [ # ; # R   Cursor position report (returned by the terminal).
*       ESC [ 6 n       Device status report (causes the above sequence to
*                               be returned).
*       ESC [ s         Save current cursor position.
*       ESC [ u         Restore the cursor position.
*       ESC [ 2 J       Erase screen and position to home (1;1).
*       ESC [ K         Erase from cursor to end of line.
*       ESC [ #;...;#m  Set attribute
*                       0  = All attributes off
*                       1  = Bold on
*                       4  = Underscore on
*                       5  = Blink on
*                       7  = Reverse video on
*                       8  = Canceled on (invisible)
*                       30 = Black foreground
*                       31 = Red foreground
*                       32 = Green foreground
*                       33 = Yellow foreground
*                       34 = Blue foreground
*                       35 = Magenta foreground
*                       36 = Cyan foreground
*                       37 = White foreground
*                       40 = Black background
*                       41 = Red background
*                       42 = Green background
*                       43 = Yellow background
*                       44 = Blue background
*                       45 = Magenta background
*                       46 = Cyan background
*                       47 = White background
*
*       ESC [ = # h     \        0  = 40x25 black and white
*       ESC [ = h        \ Set   1  = 40x25 colour
*       ESC [ = 0 h      / Mode  2  = 80x25 black and white
*       ESC [ ? 7 h     /        3  = 80x25 colour
*                                4  = 320x200 colour
*                                5  = 320x200 black and white
*                                6  = 640x200 black and white
*                                7  = Wrap at end of line.
*       ESC [ = # l     \
*       ESC [ = l        \ Reset      Same as "Set Mode" above
*       ESC [ = 0 l      / Mode       except 7.
*       ESC [ ? 7 l     /        7  = No wrap at end of line.

*               Keyboard key reassignment
*       ESC [ #;...;#p                    Examples:
*       ESC [ "string"p                   A becomes Q      ESC [65;81p
*       ESC [ #;"string";#;#;"string";#p  a becomes q      ESC [97;113p
*                                         F10 becomes "dir<CR>"
*                                               ESC [0;68;"dir";13p
*
*
      program ANSI
      character*20 name
      character ESC*1, HOME*3, ERASEEND*3
      parameter (HOME='[2J', ERASEEND='[K')
      character BLUEBACK*4, WHITFORE*4, YELFORE*4
      parameter (BLUEBACK='[44m', WHITFORE='[37m', YELFORE='[33m')

      ESC=CHAR(27)

      write(unit=*,fmt=100) ESC,HOME,ESC,BLUEBACK,ESC,WHITFORE
      do 10 i=1,22
        irow=i
        icol=i*2
        write(unit=*,fmt=110)ESC,irow,icol,irow,icol
10    continue

      write(unit=*,fmt=120)ESC,'[12;15H',ESC,ERASEEND
      read(unit=*,fmt=100)name
      write(unit=*,fmt=100)ESC,'[14;15HHello ',ESC,YELFORE,name

100   format(20A)
110   format(A,'[',I2.2,';',I2.2,'H','Row ',I2.2,' Column ',I2.2)
120   format(A,A,'What is your name:',A,A,$)

      end
