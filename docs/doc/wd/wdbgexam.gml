.chap *refid=videxam Examining and Modifying the Program State
.*
.np
The following topics are discussed:
.begbull
.bull
:HDREF refid='wndvar'.
.bull
:HDREF refid='wndmem'.
.endbull
.*
.section *refid=wndvar Variable and Watch Windows
.*
.figure *depth='2.10' *scale=70 *file='dbgvar' The Watch and Variable Window
.np
Windows that display variables come in several different varieties.  They
are:
.begbull $compact
.bull
.wnddef Locals
.bull
.wnddef File Variables
.bull
.wnddef Watches
.bull
.wnddef Variable
.endbull
.np
They are collectively called variable windows.  You use the same
interactions in all variable windows to
display, modify and browse your variables and data structures.
The only difference between these windows are the variables that
they display.  The values in each window
are updated as you trace through your program.  The windows
display the following information:
.begnote
.note Locals
Contains the list of variables which are local to the current
routine.  Choose
.menuref 'Locals' 'Data'
.ix 'variables' 'local'
to open this window.
.note File Variables
Contains a list of all variables which are defined at file scope in the
current module.  This includes external and static symbols.  Choose
.menuref 'File Variables' 'Data'
.ix 'variable' 'file scope'
to open this window.
.note Watches
The Watches windows allows you to add and delete variables and expressions.
In other windows you can choose
.popup Watch
.dot
This will open the watches window add the text which is selected in
another window to the watches window.
You can use
.popup New
.ix 'expression' 'evaluate'
:cmt. .ix 'expression' 'typecast'
.ix 'variable' 'typecast'
.ix 'typecast'
to add any expression to the Watches window.
Once entered, you can choose
.popup Edit
to edit the expressions or typecast the variables.
.note Variable
This is another instance of a Watches window.  A variable window is created
when you select a variable or expression in a window and use
.popup Inspect
.dot
.endnote
.np
Each line of a variable window has three elements.  On the left is
a button.
The button changes depending on the type of the variable or expression.
it changes based on the type of the item:
.begnote
.note structs (classes) (unions)
Structures may be opened and closed by clicking on the button at the
left. When you open a structure or class, one line is added to the
window for each field of the structure. These new lines are indented.
If you click on the button again, the structure is closed and the
window is returned to its original state.
.note arrays
Like structs, arrays may be opened and closed. When you open an array,
one line is added to the window for each element of the array. The
debugger will display at most 1000 elements of an array. If it
contains more you can use
.mi Type/Array...
.ix 'array' 'view slices'
.ix 'array' 'expand'
to open different ranges. Multi dimensional arrays are treated like an
array of arrays. When you open the first dimension, the lines that are
added will also be arrays which you can open.
.note pointers
.ix 'pointer' 'follow'
When the variable is a pointer, you can click on the button and the
debugger will follow the pointer and change the line to represent the
item which is the result of the pointer reference. For example, if you
have a pointer to an integer and click on the button, the integer
value will be displayed. The button then changes to indicate so that
you can undo the operation by clicking on it again.
.np
In the case of pointers to pointers, clicking on the button will
follow the pointers one level each time you click on the button until
a non-pointer value is reached. Clicking on the button at this point
will undo take you back to the original state. When the pointer points
to a struct, the structure will automatically be opened when you click
on the button. If a pointer is really an array, you can use
.popup Type/Array...
.ix 'pointer' 'show as array'
to open it as an array.
.endnote
.np
Next comes the name of the variable, field or array element being
displayed. Finally, the value is displayed. If the item being
displayed is not a scalar item, the value displayed is an indication
that it is a complex data type. If the value changes when you run your
program, it will be highlighted. If a variable goes out of scope, or a
pointer value becomes invalid, the value will be displayed as question
marks.
.np
.ix 'variable' 'modify'
You can modify a variable's value by double clicking on the value
field, or by cursoring to it and pressing enter. Double clicking or
pressing enter on the name field is equivalent to clicking on the
button. Press the right mouse button to access the following pop-up
menu items:
.begmenu Variable Pop-up
.menuitem Modify...
Modify the value of the selected item.
.menuitem Break
.ix 'variable' 'break on write'
.ix 'breakpoint' 'on write'
Set a breakpoint so that execution stops when the selected item's
value changes.  This is the same as setting a breakpoint on the
object.
.refalso vidbrk
.note --------
.menuitem Inspect
.ix 'variable' 'inspect'
Open a new Variable window containing the selected item.
If the item is a compound object (array, class, or structure), it
will be opened automatically.
.menuitem Watch
.ix 'variable' 'watch'
Add the selected item to the Watch window.
.menuitem Show/Raw Memory
.ix 'variable' 'show raw storage'
Display raw memory at the address of this variable.
This lets you examine the actual binary representation of a variable.
.menuitem Show/Pointer Memory
.ix 'pointer' 'show memory at'
Display the memory that the item points to.
This is useful when you have a pointer to a block of memory that does
not have a type associated
with it.
.menuitem Show/Pointer Code
.ix 'pointer' 'show code at'
Display the code that the variable points to.
If the item being displayed is a pointer to function, you can use this
menu item to see the definition of that function.
.menuitem Show/Type
.ix 'type' 'show item's type'
.ix 'variable' 'display type'
Display the type of the variable in an information message box.
Select "OK" to dismiss the information box and resume debugging.
.note --------
.menuitem Edit
.ix 'typecast'
.ix 'variable' 'typecast'
Open a dialog box in which you can edit an expression in the Watch
window.  This is useful for typecasting variables or evaluating
expressions.
.refalso videxpr
.menuitem New
Add a new variable or expression to the window. You will be prompted
for the expression to add.
.menuitem Delete
Delete the selected item from the window.
.note --------
:CMT. .menuitem Root
:CMT. .ix 'structure' 'find root of'
:CMT. When the selected item is a field of a structure or an element of an
:CMT. array, this menu item moves the currently selected item to the
:CMT. parent of that item.  For example, if you are examining
:CMT. .monoon
:CMT. sym->field1->field2
:CMT. .monooff
:CMT. .mi Root
:CMT. will change the current selection to
:CMT. .monoon
:CMT. sym->field1
:CMT. .monooff
:CMT. If you are examining a deeply nested field, use Root repeatedly
:CMT. to find the originating expression.
.menuitem FieldOnTop
.ix 'on top'
Display the value of this member at the top of the structure/class.
You can selectively add or remove items from the list that is
displayed "on top".
For example, say you have a
.id struct Point
displayed as:
.millust begin
[-] point
  x      10
  y      30
  other  "asdf"
.millust end
.pc
If you toggle
.mi FieldOnTop
for both
.id x
and
.id y
then
.id point
would be displayed like this:
.millust begin
[-] point { 10, 30 }
  x      10
  y      30
  other  "asdf"
.millust end
.pc
Furthermore, if you closed the struct (or pointer to struct) then
you would see:
.millust begin
[+] point { 10, 30 }
.millust end
.np
This carries to structs containing structs (and so on) as shown in the
following struct containing two
.id Point
structures.
.millust begin
[-] rect { { 10, 10 }, { 30, 30 } }
  top_left { 10, 10 }
  bot_right { 30, 30 }
.millust end
.pc
If you close it, then you will see:
.millust begin
[+] rect { { 10, 10 }, { 30, 30 } }
.millust end
.menuitem Class/Show Functions
Display function members of this object.
If this option is not selected, no functions are displayed.
This option works in conjunction with other
.mi Class
selections to display "Inherited", "Generated", "Private" and
"Protected" functions.
.menuitem Class/Show Inherited
Display inherited members of this object.
To see inherited functions, you must also select
.mi Class/Show Functions.
.menuitem Class/Show Generated
Display compiled-generated members of this object.
To see generated functions, you must also select
.mi Class/Show Functions.
.menuitem Class/Show Private
Display private members of this object.
To see private functions, you must also select
.mi Class/Show Functions.
.menuitem Class/Show Protected
Display protected members of this object.
To see protected functions, you must also select
.mi Class/Show Functions.
.menuitem Class/Show Static
Display static members of this object.
.menuitem Type/All Hex
This item is only available when the display item is an array or a fake 
array (a pointer changed to display as if it were an array using
.mi Type/Array
). Change the value of all sibling array entries to be displayed in hexadecimal.
.menuitem Type/Hex
Change the value to be displayed in hexadecimal.
.menuitem Type/All Decimal
This item is only available when the display item is an array or a fake 
array (a pointer changed to display as if it were an array using
.mi Type/Array
). Change the value of all sibling array entries to be displayed in decimal.
.menuitem Type/Decimal
Change the value to be displayed in decimal.
.menuitem Type/Character
Change the value to be displayed as a single character constant.  This
useful when you have a one byte variable that really contains a character.
The debugger will often display it as an integer by default.
.menuitem Type/String
.ix 'pointer' 'display as string'
.ix 'string' 'display pointer as'
The debugger automatically detects pointers to strings in the variable windows
and displays the string rather than the raw pointer value.  In the string is
not null terminated, contains non-printable characters, or is not typed as
a pointer to 'char', this mechanism will not work.
.mi Type/String
overrides the automatic string detecting and displays the pointer as a
string regardless of its type.
.menuitem Type/Pointer
.ix 'pointer' 'display value'
.ix 'string' 'display pointer'
This will undo the effects of
.mi Type/String
or
.mi Type/Array
.dot
It will also let you see the raw pointer value when the debugger has
automatically displayed a pointer to char as a string.
.menuitem Type/Array...
.ix 'pointer' 'display as array'
Use this menu item to display a pointer as if it were an array, or to
display ranges of an array's elements.  You will be prompted for the
first and last element to display.
.menuitem Options/Whole Expression
Select this option to show the whole expression used to access fields
and array elements instead of just the element number or field name
itself.
.menuitem Options/Expand 'this'
Do not display members of the 'this' pointer as if they were local
variables declared within the member function.
.endmenu
.*
.section *refid=wndmem The Memory and Stack Windows
.*
.figure *depth='2.50' *scale=58 *file='dbgmem' The Memory Window
.np
Use the
.wnddef Memory
window or the
.wnddef Stack
window to examine memory in raw form.
.ix 'memory' 'display'
To open a Memory window, choose
.menuref 'Memory At' 'Data'
.dot
The
.us Enter Memory Address
dialog appears. Enter the memory address and press Return to see the
Memory window.
You can also use one of the Show/Pointer Memory or Show/Raw Memory
items in a variable window to display the memory associated with a
variable.
.np
The Stack Window always shows the memory at the stack pointer.  It is moved
as your program executes to track the top of the stack.  The stack
pointer location will be at the top of the window.  The location
of the BP or EBP register will also be indicated.
.ix 'stack' 'display'
Choose
.menuref 'Stack' 'Data'
to open the Stack window.
.np
.ix 'memory' 'modify'
You can modify memory by double-clicking on a value in the Memory or
Stack window, or by cursoring to it and pressing enter. You will be
prompted for a new value.
.np
Memory windows allow you to follow data structures in the absence of
debugging information.  The
.mi Follow
menu items will reposition the memory window to the address that is
found under the cursor.  The
.mi Repeat
and
.mi Previous
items will let you repeat a follow action.  This makes it simple
to follow linked lists.
Press the right mouse button to access the following pop-up menu items:
.begmenu Memory Pop-up
.menuitem Modify
.ix 'memory' 'modify'
Modify the value at the selected address.  You will be prompted for
a new value.  You should enter the value in the same radix as the window is
currently displaying.  You are not limited to typing constants values.
You can enter an arbitrary expression to be used for the new value.
.menuitem Break on Write
.ix 'memory' 'break on write'
.ix 'breakpoint' 'on write'
Set a breakpoint to stop execution when the selected value changes.
.refalso vidbrk
.note --------
.menuitem Near Follow
.ix 'memory' 'follow pointers'
.ix 'pointer' 'follow in memory'
Displays the memory that the selected memory points to,
treating it as a near pointer.  The new offset to be displayed will
be xxxx where xxxx is the word under the cursor.
DGROUP will be used as the segment if it can be located.
The program's initial stack segment will be used otherwise.
When you are debugging a
16-bit or 32-bit application, the appropriate word size is used.
.menuitem Far Follow
Displays the memory that the selected memory points to,
treating it as a far pointer.  The new address to be displayed will
be the the segment and offset found at the cursor location.  Note
that pointers are stored in memory with the offset value first and the
segment value second.
.menuitem Segment Follow
Display the segment that the selected memory points to,
treating it as a segment selector.  The new address to be displayed will
be xxxx:0 where xxxx is the two byte word under the cursor.
.menuitem Cursor Follow
.ix 'memory' 'examine array'
Make the selected position the new starting address in
the window. This means that the first byte in the memory
window will become the byte that the cursor was pointing
to.  This is useful for navigating through an array when no debugging
information is available.
.menuitem Repeat
Repeat the previous Follow operation.  The new address that will
be used is at the same offset relative to the beginning of the window
as it was in the original Follow operation.   Repeating a pointer or
segment follow is a linked list traversal.  Repeating a Cursor Follow
operation advances to the next element in an array.
.menuitem Previous
Back out of a Follow or Repeat operation.  This
will display the memory window you were previously viewing.
Essentially, this undoes a Follow operation.  You can back
all the way out to the first memory location you were examining.
.menuitem Home
Undo all Follow and Repeat operations.  This will take you back
to the very first location window you were examining.  It is equivalent
to using Previous repeatedly.
.note --------
.menuitem Left
Scroll the window backward through memory by the size of
the displayed memory items.
.menuitem Right
Scroll the window forward through memory by the size of
the displayed memory items.
.note --------
.menuitem Address
.ix 'memory' 'examine new address'
Position the window at a new address.  You will be prompted to type in
a new address.  You can type an arbitrary expression.
.refalso videxpr
If you type the
name of a variable, the
.us address
of that variable is used.  If the expression you type does not contain
a segment value DGROUP will be used as the segment if it can be located.
The program's initial stack segment will be used otherwise.
.menuitem Assembly
Position the assembly window to the address of the memory under the cursor.
This is useful if you have incorrectly displayed a pointer as data and wish
to look at the code instead.
.menuitem Type/Byte
.ix 'memory' 'set display type'
Display as hexadecimal bytes.
.menuitem Type/Word
Display as hexadecimal 16-bit words.
.menuitem Type/Dword
Display as hexadecimal 32-bit words.
.menuitem Type/Qword
Display as hexadecimal 64-bit words.
.menuitem Type/Char
Display as signed 8-bit integers.
.menuitem Type/Short
Display as signed 16-bit integers.
.menuitem Type/Long
Display as signed 32-bit integers.
.menuitem Type/__int64
Display as signed 64-bit integers.
.menuitem Type/Unsigned Char
Display as unsigned 8-bit integers.
.menuitem Type/Unsigned Short
Display as unsigned 16-bit integers.
.menuitem Type/Unsigned Long
Display as unsigned 32-bit integers.
.menuitem Type/Unsigned __int64
Display as unsigned 64-bit integers.
.menuitem Type/0:16 Pointer
Display as 16-bit near pointers (16-bit offset).
.menuitem Type/16:16 Pointer
Display as 32-bit far pointers (16-bit segment, 16-bit offset).
.menuitem Type/0:32 Pointer
Display as 32-bit near pointers (32-bit offset).
.menuitem Type/16:32 Pointer
Display as 48-bit far pointers (16-bit segment, 32-bit offset).
.menuitem Type/Float
Display as 32-bit floating-point values.
.menuitem Type/Double
Display as 64-bit floating-point values.
.menuitem Type/Extended Float
Display as 80-bit floating-point values.
.endmenu
.*
.beglevel
.*
.section Following Linked Lists
.*
.np
.ix 'linked lists' 'following in memory'
Use the memory window to display the memory address of the
first node of your linked list. Move to the "next" field of your
structure and use the Near (or Far)
Follow command. The next node of your linked list will be
displayed. Now by using the Repeat
command you can traverse the linked list.
.*
.section Traversing Arrays
.*
.np
.ix 'array' 'traversing in memory'
Display the memory address of
your array. Select the first byte of the second element
of your array then use the Cursor Follow command to move
the second element of your array to the beginning of
the memory window. By using the Repeat
command you can traverse your array.
.*
.endlevel
