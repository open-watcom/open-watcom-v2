.chap *refid=vidasm Assembly Level Debugging
.*
.ix 'assembly' 'debugging'
.ix 'debugging' 'at assembly level'
This chapter addresses the following assembly language level debugging
features:
.begbull
.bull
:HDREF refid='wndreg'.
.bull
:HDREF refid='wndasm'.
.bull
:HDREF refid='wndio'.
.bull
:HDREF refid='wndfpu'.
.bull
:HDREF refid='wndmmx'.
.bull
:HDREF refid='wndxmm'.
.endbull
.*
.section *refid=wndreg The CPU Register Window
.*
.figure *depth='1.70' *scale=70 *file='dbgreg' The CPU Register Window
.*
.*
.np
.ix 'registers' 'examining'
You can open the
.wnddef CPU Register
window by choosing
.menuref 'Register' 'Data'
.dot
The register names and values are
displayed in this window.  As you execute your program,
registers that have changed since the last trace or breakpoint
will be highlighted.
.np
.ix 'registers' 'modifying'
You can modify a register value by double clicking on
the value, or by cursoring to it and pressing ENTER.
Press the right mouse button to access the following pop-up menu items:
.begmenu Register Pop-up
.menuitem Modify
Change the value of the selected register.
.menuitem Inspect
.ix 'registers' 'displaying memory'
Open a Memory window displaying the memory contents of the
address specified by the register.  If a segment register
is selected, memory at offset 0 in the segment will
be displayed.
.menuitem Hex
.ix 'registers' 'displaying in decimal'
Toggles the register window display format between hexadecimal and decimal.
.menuitem Extended
.ix 'registers' 'displaying 32-bit'
Displays the Extended 386 register set.
:INCLUDE file='wdbgopt.gml'.
.endmenu
.*
.section *refid=wndasm The Assembly Window
.*
.figure *depth='3.66' *scale=60 *file='dbgass' The Assembly Window
.np
You can open the
.wnddef Assembly
window by choosing
.menuref 'Assembly' 'Code'
.dot
You can Inspect an item in by
double-clicking on it, or by cursoring to it and pressing ENTER.
Press the right mouse button to access the following pop-up menu items:
.begmenu Assembly Pop-up
.menuitem Inspect
.ix 'assembly' 'inspecting operands'
When you selecting a memory address, register or operand and use Inspect,
the debugger opens a Memory Window displaying the selected
memory address.
.menuitem Break
.ix 'assembly' 'setting break points'
.ix 'breakpoint' 'in assembly code'
If a code address is selected this command will set a
break-on-execute breakpoint at the selected code address.  If a variable
address is selected, this command will set a break-on-write
breakpoint on the selected address.
.bi this does not set a break at the current line.
Use
.menuref 'Toggle' 'Break'
or
.menuref 'At Cursor' 'Break'
to set a breakpoint at the current line.
.menuitem Enter Function
Resume program execution until the selected function is executed.
.menuitem Show/Source
Display the source code associated with the selected
assembly line.
.menuitem Show/Functions
Show the list of all functions defined in the current module.
.menuitem Show/Address
.ix 'assembly' 'examining'
Reposition the window at a new address.  You will be prompted for an
expression.  Normally you would type a function name but you can type
any expression that resolves to a code address. For example, you might
type the name of a variable that contains a pointer to a function.
.refalso videxpr
.menuitem Show/Module...
Show a different module.  You will be prompted for its name in a dialog.
As a shortcut,  you can type the beginning of a module name and click
the Module... button.  This will display a list of all modules that
start with the text you typed.
.menuitem Home
Reposition the window to the currently executing location. The cursor will move to
the next line of the program to be executed.
.menuitem No source
Toggle the Assembly window display between only assembly code and assembly 
code intermixed with source lines.
:INCLUDE file='wdbgopt.gml'
.menuitem Hex
Toggle the Assembly window display between hexadecimal and decimal.
:INCLUDE file='wdbgopt.gml'
.endmenu
.*
.section *refid=wndio The I/O Ports Window
.*
.figure *depth='1.50' *scale=70 *file='dbgio' The I/O Window
.np
Use the
.wnddef I/O
window to manipulate I/O ports.  This is only supported
when the operating system allows application software
to use
.bd IN
and
.bd OUT
instructions.
.ix 'I/O ports' 'reading'
.ix 'I/O ports' 'writing'
I/O ports can be added to the window, and typed as
a byte, word (2 bytes) or dword (4 bytes).  Use
.popup New
to add a new port to the window. Once you have done this, four items
will appear on the line. First appears the read button which appears
as an open book, or [r]. Second appears the write button. It is a
pencil or [w]. Third appears the port address, and finally the value.
When you first enter a port address the value appears as question
marks. The debugger does not automatically read or write the value
since this can have side effects. In order to read the displayed value
from the port, click on the read button. To write the displayed value
back, click on the write button. You can change the value by double
clicking on it, or by cursoring to it and pressing ENTER. Press the
right mouse button to access the following pop-up menu items:
.begmenu I/O Pop-up
.menuitem Modify
Change the selected item. You can change either the value field or the
address field.
.bi This does not write the value back to the port.
You must choose
.mi Write
to write to the port.
.menuitem New
Add a new line to the window.  You can have several I/O ports displayed
at once.
.menuitem Delete
Delete the selected line from the window.
.menuitem Read
Read the displayed value from the port.
.menuitem Write
Write the displayed value to the port.
.menuitem Type
Change the display type of the value.  The size of this type determines
how much is read from or written to the I/O port.
.endmenu
.*
.section *refid=wndfpu The FPU Registers Window
.*
.figure *depth='1.20' *scale=48 *file='dbgfpu' The FPU Registers Window
.np
.ix '8087' 'examining'
.ix '8087' 'modifying'
.ix '387' 'examining'
.ix '387' 'modifying'
.ix 'Coprocessor' 'examining'
.ix 'Coprocessor' 'modifying'
Choose
.menuref 'FPU Registers' 'Data'
to open the
.wnddef FPU
window. This window displays the current value and status of all the
FPU registers.
If you are debugging a program that uses Intel 8087 emulation, this
window display the contents of the emulator's data area.
You can change a value by double-clicking on, it or by cursoring to it
and pressing ENTER.
Press the right mouse button to access the following pop-up menu items:
.begmenu FPU Pop-up
.menuitem Modify
Change the value of the selected register, or bit.  You will
be prompted for a new value, unless you are modifying a bit.
A bit will toggle between 0 and 1.
.menuitem Hex
Toggle the FPU window display between hexadecimal and floating-point display.
:INCLUDE file='wdbgopt.gml'
.endmenu
.*
.section *refid=wndmmx The MMX Registers Window
.*
.figure *depth='1.80' *scale=70 *file='dbgmmxb' The MMX Registers Window
.np
.ix 'MMX' 'examining'
.ix 'MMX' 'modifying'
.ix 'Multi-media extension registers' 'examining'
.ix 'Multi-media extension registers' 'modifying'
Choose
.menuref 'MMX Registers' 'Data'
to open the
.wnddef MMX
window.
This window displays the current values of all the MMX
registers.
You can change a value by double-clicking on, it or by cursoring to it
and pressing ENTER.
Press the right mouse button to access the following pop-up menu items:
.begmenu MMX Pop-up
.menuitem Modify
Change the value of the selected register component.
You will be prompted for a new value.
The same action can be performed by pressing ENTER or double-clicking
as described above.
.menuitem Inspect
This item has no function in the MMX register window.
.menuitem Hex
Toggle the MMX register window display between hexadecimal and
floating-point display.
:INCLUDE file='wdbgopt.gml'
.menuitem Signed
Toggle the display of the contents of the MMX registers as signed or
unsigned quantities.
When "signed" is enabled, each byte, word, doubleword or quadword
is displayed as a signed quantity.
When "signed" is disabled, each byte, word, doubleword or quadword
is displayed as an unsigned quantity.
.menuitem Byte
Display the contents of the MMX registers as a series of 8 bytes.
.menuitem Word
Display the contents of the MMX registers as a series of 4 words.
.menuitem DWord
Display the contents of the MMX registers as a series of 2 doublewords.
.menuitem QWord
Display the contents of the MMX registers as single quadwords.
.menuitem Float
Display the contents of the MMX registers as a series of 2 IEEE
single-precision floating-point values.
.endmenu
.*
.section *refid=wndxmm The XMM Registers Window
.*
.figure *depth='1.74' *scale=55 *file='dbgxmmb' The XMM Registers Window
.np
.ix 'XMM' 'examining'
.ix 'XMM' 'modifying'
.ix 'XMM/SSE registers' 'examining'
.ix 'XMM/SSE registers' 'modifying'
Choose
.menuref 'XMM Registers' 'Data'
to open the
.wnddef XMM
window.
This window displays the current values of all the XMM
registers, as well as the contents of XMM status registers.
You can change a value by double-clicking on, it or by cursoring to it
and pressing ENTER.
Press the right mouse button to access the following pop-up menu items:
.begmenu XMM Pop-up
.menuitem Modify
Change the value of the selected register component.
You will be prompted for a new value.
The same action can be performed by pressing ENTER or double-clicking
as described above.
.menuitem Inspect
This item has no function in the XMM register window.
.menuitem Hex
Toggle the XMM register window display between hexadecimal and
floating-point display.
:INCLUDE file='wdbgopt.gml'
.menuitem Signed
Toggle the display of the contents of the XMM registers as signed or
unsigned quantities.
When "signed" is enabled, each byte, word, doubleword or quadword
is displayed as a signed quantity.
When "signed" is disabled, each byte, word, doubleword or quadword
is displayed as an unsigned quantity.
.menuitem Byte
Display the contents of the XMM registers as a series of 16 bytes.
.menuitem Word
Display the contents of the XMM registers as a series of 8 words.
.menuitem DWord
Display the contents of the XMM registers as a series of 4 doublewords.
.menuitem QWord
Display the contents of the XMM registers as a series of 2 quadwords.
.menuitem Float
Display the contents of the XMM registers as a series of 4 single-precision
floating-point values.
.menuitem Double
Display the contents of the XMM registers as a series of 2 double-precision
floating-point values.
.endmenu
