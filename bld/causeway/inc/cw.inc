;-------------------------------------------------------------------------------
;
;Call a CauseWay system function. Operand supplied is the function number. Use
;of this macro is not obligatory, use your own version if you want. These are
;the low level functions. Other API emulations come later.
;
Sys     macro p1
        mov     ax,p1
        int     31h
        endm


;-------------------------------------------------------------------------------
;
;Get system selectors/flags.
;
;On Exit:
;
;AX     - Selector for real mode segment address of 00000h, 4G limit.
;BX     - Selector for current PSP segment. 100h limit.
;[E]CX  - Dos transfer buffer size. Always <64k.
;DX     - Dos transfer buffer real mode segment address.
;ES:[E]SI       - Dos transfer buffer protected mode address. ESI+ECX ALWAYS <64k.
;EDI    - System flags. Bits significant if set.
;
;       0 - 32 bit code default.
;
;       1 - Virtual memory manager functional.
;
;       2 \ Mode, 0 - raw, 1 - VCPI, 2 - DPMI.
;       3 /
;
;       4 - DPMI available.
;       5 - VCPI available.
;       6 - No memory managers.
;
;       7 - Descriptor table type. 0 - GDT, 1 - LDT.
;
;NOTES:
;
;Bits 1-2 of DI indicate the interface type being used by CauseWay. Bits 4-5
;indicate the interface types that are available.
;
;Bit 7 indicates the descriptor table being used to allocate selectors to the
;application when on a raw/vcpi system.
;
;The Dos transfer buffer is the area CauseWay uses to transfer data between
;conventional and extended memory during DOS interupts. This memory can be used
;as temporary work space for your own access to real mode code as long as you
;remember it may be over written the next time you issue an INT in protected
;mode that requires segment&|offset pointers.
;
Info    equ     0ff00h          ;Get general purpose segment selectors.


;-------------------------------------------------------------------------------
;
;Simulate real mode interupt.
;
;On Entry:
;
;BL     - Interupt number.
;ES:[E]DI       - Parameter block.
;
;On Exit:
;
;Parameter block updated.
;
;NOTES:
;
;The parameter block referanced by ES:[E]DI should contain the register values
;you want passing to the real mode interupt handler. The SS:SP & Flags entries
;are currently filled in by CauseWay to ensure legal values are used and the
;CS:IP entries are ignored. This function bypasses protected mode interupt
;handlers and provides access to INT API's that would not otherwise be available
;using CauseWay.
;
IntXX   equ     0ff01h


;-------------------------------------------------------------------------------
;
;Simulate real mode far call.
;
;On Entry:
;
;ES:[E]DI       - Parameter block.
;
;On Exit:
;
;Parameter block updated.
;
;NOTES:
;
;This function works much the same as IntXX but provides a 16 bit FAR stack
;frame and the CS:IP values used to pass control to the real mode code.
;
FarCallReal     equ     0ff02h


;-------------------------------------------------------------------------------
;
;Allocate real mode call back address.
;
;On Entry:
;
;DS:[E]SI       - Call address.
;ES:[E]DI       - Real mode register structure.
;
;On Exit:
;
;Carry set on error, else,
;
;CX:DX  - Real mode address to trigger mode switch.
;
;NOTES:
;
;Real mode CallBack's provide a means of switching from real mode to protected
;mode. The address returned by this function is a unique real mode address that
;when given control in real mode will switch to protected mode and pass control
;to the protected mode routine supplied at entry to this function.
;
GetCallBack     equ     0303h


;-------------------------------------------------------------------------------
;
;Release a real mode call back entry.
;
;On Entry:
;
;CX:DX  - Real mode address returned by GetCallBack
;
RelCallBack     equ     0304h


;-------------------------------------------------------------------------------
;
;Get Protected mode interupt handler address.
;
;On Entry:
;
;BL     - Interupt vector number.
;
;On Exit:
;
;CF set on error else:-
;
;CX:[E]DX       - selector:offset of handler.
;
GetVect equ     0204h


;-------------------------------------------------------------------------------
;
;Set Protected mode interupt handler address.
;
;On Entry:
;
;BL     - Interupt vector number.
;CX:[E]DX       - selector:offset of new handler.
;
;On Exit:
;
;CF set on error.
;
SetVect equ     0205h


;-------------------------------------------------------------------------------
;
;Get real mode interupt handler address.
;
;On Entry:
;
;BL     - Interupt vector number.
;
;On Exit:
;
;CF set on error else:-
;
;CX:DX  - selector:offset of handler.
;
GetRVect        equ     0200h


;-------------------------------------------------------------------------------
;
;Set real mode interupt handler address.
;
;On Entry:
;
;BL     - Interupt vector number.
;CX:DX  - selector:offset of new handler.
;
;On Exit:
;
;CF set on error.
;
SetRVect        equ     0201h


;-------------------------------------------------------------------------------
;
;Get Protected mode exception handler address.
;
;On Entry:
;
;BL     - Exception vector number.
;
;On Exit:
;
;CF set on error else:-
;
;CX:[E]DX       - selector:offset of handler.
;
GetEVect        equ     0202h


;-------------------------------------------------------------------------------
;
;Set Protected mode exception handler address.
;
;On Entry:
;
;BL     - Exception vector number.
;CX:[E]DX       - selector:offset of new handler.
;
;On Exit:
;
;CF set on error.
;
SetEVect        equ     0203h


;-------------------------------------------------------------------------------
;
;Allocate a new selector.
;
;On Exit:
;
;BX     - Selector.
;
;NOTES:
;
;A selector is allocated and initialised with a base of 0, a limit of 0 and as
;read/write expand up data. Use SetSelDet to make the selector useful.
;
GetSel  equ     0ff03h


;-------------------------------------------------------------------------------
;
;Allocate multiple selectors.
;
;On Entry:
;
;CX     - Number of selectors.
;
;On Exit:
;
;BX     - Base selector.
;
;NOTES:
;
;The selectors are allocated and initialised with a base of 0, a limit of 0 and
;as read/write expand up data. Use SetSelDet to make the selectors useful.
;
GetSels equ     0ff29h


;-------------------------------------------------------------------------------
;
;Release a selector.
;
;On Entry:
;
;BX     - Selector.
;
RelSel  equ     0ff04h


;-------------------------------------------------------------------------------
;
;Make a selector execute/read type.
;
;On Entry:
;
;BX     - Selector.
;CL     - Default operation size. (0=16 bit,1=32 bit)
;
;NOTES:
;
;Allows a selector to be converted to a type suitable for execution.
;
CodeSel equ     0ff05h


;-------------------------------------------------------------------------------
;
;Create a read/write data selector with same base and limit as selector
;supplied.
;
;On Entry:
;
;BX     - Source selector
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;AX     - New data selector
;
;If function was not successful:
;Carry flag is set.
;
;NOTES:
;
;This function always creates a read/write data selector regardless of the
;source selectors type. Can be used to provide access to variables in a code
;segment etc.
;
AliasSel        equ     0ff06h


;-------------------------------------------------------------------------------
;
;Get selector linear base and limit.
;
;On Entry:
;
;BX     - Selector
;
;On Exit:
;
;CX:DX  - Linear base.
;SI:DI  - Byte granular limit.
;
GetSelDet       equ     0ff07h


;-------------------------------------------------------------------------------
;
;Get selector linear base and limit.
;
;On Entry:
;
;BX     - Selector
;
;On Exit:
;
;EDX    - Linear base.
;ECX    - Byte granular limit.
;
GetSelDet32     equ     0ff08h


;-------------------------------------------------------------------------------
;
;Set selector linear base and limit.
;
;On Entry:
;
;BX     - Selector.
;CX:DX  - Linear base.
;SI:DI  - Byte granular limit.
;
SetSelDet       equ     0ff09h


;-------------------------------------------------------------------------------
;
;Set selector linear base and limit.
;
;On Entry:
;
;BX     - Selector.
;EDX    - Linear base.
;ECX    - Byte granular limit.
;
SetSelDet32     equ     0ff0ah


;-------------------------------------------------------------------------------
;
;Allocate a block of memory.
;
;On Entry:
;
;CX:DX  - Size of block required in bytes. (-1:-1 to get maximum memory size)
;
;On Exit:
;
;Carry clear if OK &
;
;BX     - Selector to access the block with.
;
;Else if CX:DX was -1, CX:DX is size of largest block available.
;
;NOTES:
;
;This function allocates a block of extended (application) memory, and also a
;selector with a suitable base & limit.
;
GetMem  equ     0ff0bh


;-------------------------------------------------------------------------------
;
;Allocate some memory and return selector:offset (16-bit).
;
;On Entry:
;
;CX:DX  - Size of block required in bytes.
;
;On Exit:
;
;Carry set on error else,
;
;SI:DI  - selector:offset of allocated memory.
;
GetMemSO        equ     0ff2ch


;-------------------------------------------------------------------------------
;
;Allocate a block of memory.
;
;On Entry:
;
;ECX    - Size of block required in bytes. (-1 to get maximum memory size)
;
;On Exit:
;
;Carry clear if OK &
;
;BX     - Selector to access the block with.
;
;Else if ECX was -1, ECX is size of largest block available.
;
;NOTES:
;
;This function allocates a block of extended (application) memory, and also a
;selector with a suitable base & limit.
;
GetMem32        equ     0ff0ch


;-------------------------------------------------------------------------------
;
;Re-size a previously allocated block of memory.
;
;On Entry:
;
;BX     - Selector for block.
;CX:DX  - New size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK.
;
;NOTES:
;
;If the memory block can't be re-sized in its current location, but a free block
;of memory of the new size exists, the memory will be copied to a new block and
;the old one released. This is transparent to the application as long as only
;the selector originaly allocated with GetMem is being used to access the
;memory.
;
ResMem  equ     0ff0dh


;-------------------------------------------------------------------------------
;
;Re-size a block of memory allocated via GetMemSO.
;
;On Entry:
;
;SI:DI  - Selector:offset for block.
;CX:DX  - New size of block required in bytes.
;
;On Exit:
;
;Carry set on error else,
;
;SI:DI  - selector:offset new block address.
;
ResMemSO        equ     0ff2dh


;-------------------------------------------------------------------------------
;
;Re-size a previously allocated block of memory.
;
;On Entry:
;
;BX     - Selector for block.
;ECX    - New size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK.
;
;NOTES:
;
;If the memory block can't be re-sized in its current location, but a free block
;of memory of the new size exists, the memory will be copied to a new block and
;the old one released. This is transparent to the application as long as only
;the selector originaly allocated with GetMem is being used to access the
;memory.
;
ResMem32        equ     0ff0eh


;-------------------------------------------------------------------------------
;
;Release previously allocated block of memory. Is be used for memory allocated
;by either GetMem or GetMem32.
;
;
;On Entry:
;
;BX     - Selector for block to release.
;
RelMem  equ     0ff0fh


;-------------------------------------------------------------------------------
;
;Release block of memory allocated via GetMemSO.
;
;On Entry:
;
;SI:DI  - Selector:offset for block to release.
;
RelMemSO        equ     0ff2eh


;-------------------------------------------------------------------------------
;
;Allocate a block of memory without a selector.
;
;On Entry:
;
;CX:DX  - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;SI:DI  - Linear address of block allocated.
;
;NOTES:
;
;Addresses returned by this function may be >16M
;
GetMemLinear    equ     0ff10h


;-------------------------------------------------------------------------------
;
;Allocate a block of memory without a selector.
;
;On Entry:
;
;ECX    - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - Linear address of block allocated.
;
;NOTES:
;
;Addresses returned by this function may be >16M
;
GetMemLinear32 equ      0ff11h


;-------------------------------------------------------------------------------
;
;Re-size a previously allocated block of memory without a selector.
;
;On Entry:
;
;SI:DI  - Linear address of block to re-size.
;CX:DX  - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;SI:DI  - New linear address of block.
;
;NOTES:
;
;If the memory block cannot be expanded to the desired size, and a free block
;of sufficient size exists, the existing memory will be copied to the free
;block and released, the new block then being alocated in place of the old.
;
ResMemLinear    equ     0ff12h


;-------------------------------------------------------------------------------
;
;Re-size a previously allocated block of memory without a selector.
;
;On Entry:
;
;ESI    - Linear address of block to re-size.
;ECX    - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - New linear address of block.
;
;NOTES:
;
;If the memory block cannot be expanded to the desired size, and a free block
;of sufficient size exists, the existing memory will be copied to the free
;block and released, the new block then being alocated in place of the old.
;
ResMemLinear32 equ      0ff13h


;-------------------------------------------------------------------------------
;
;Release previously allocated block of memory (linear address).
;
;On Entry:
;
;SI:DI  - Linear address of block to release.
;
;On Exit:
;
RelMemLinear    equ     0ff14h


;-------------------------------------------------------------------------------
;
;Release previously allocated block of memory (linear address).
;
;On Entry:
;
;ESI    - Linear address of block to release.
;
;On Exit:
;
RelMemLinear32 equ      0ff15h


;-------------------------------------------------------------------------------
;
;Allocate an application relative block of memory.
;
;On Entry:
;
;EBX    - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - Application relative linear address of block allocated.
;
;NOTES:
;
;Addresses returned by this function are as an offset from the application.
;
GetMemNear      equ     0ff16h


;-------------------------------------------------------------------------------
;
;Re-size a previously allocated application relative block of memory.
;
;On Entry:
;
;EBX    - Size of block required in bytes.
;ESI    - application relative linear address of block to re-size.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - New application relative linear address of block.
;
;NOTES:
;
;If the memory block cannot be expanded to the desired size, and a free block
;of sufficient size exists, the existing memory will be copied to the free
;block and released, the new block then being allocated in place of the old.
;
ResMemNear      equ     0ff17h


;-------------------------------------------------------------------------------
;
;Release previously allocated application relative block of memory.
;
;On Entry:
;
;ESI    - Application relative linear address of block to release.
;
;On Exit:
;
RelMemNear      equ     0ff18h


;-------------------------------------------------------------------------------
;
;Convert linear address to application relative address.
;
;On Entry:
;
;ESI    - Linear address to convert.
;
;On Exit:
;
;ESI    - Application relative linear address.
;
Linear2Near     equ     0ff19h


;-------------------------------------------------------------------------------
;
;Convert application relative address to linear address.
;
;On Entry:
;
;ESI    - Application relative linear address.
;
;On Exit:
;
;ESI    - Linear address to convert.
;
Near2Linear     equ     0ff1ah


;-------------------------------------------------------------------------------
;
;Lock a region of memory.
;
;On Entry:
;
;BX:CX  - Starting linear address of memory to lock.
;SI:DI  - Size of region to lock in bytes.
;
;On Exit:
;
;Carry set on error, none of the memory locked, else memory is locked.
;
;NOTES:
;
;Memory that is locked cannot be swapped to disk by the VMM. You should note
;that locking is applied to memory on 4k boundaries, so areas of memory below
;and above the memory being locked will also be locked if the specified region
;is not 4k aligned.
;
LockMem equ     0ff1bh


;-------------------------------------------------------------------------------
;
;Lock a region of memory.
;
;On Entry:
;
;ESI    - Starting linear address of memory to lock.
;ECX    - Size of region to lock in bytes.
;
;On Exit:
;
;Carry set on error, none of the memory locked, else memory is locked.
;
;NOTES:
;
;Memory that is locked cannot be swapped to disk by the VMM. You should note
;that locking is applied to memory on 4k boundaries, so areas of memory below
;and above the memory being locked will also be locked if the specified region
;is not 4k aligned.
;
LockMem32       equ     0ff1ch


;-------------------------------------------------------------------------------
;
;Un-lock a region of memory.
;
;On Entry:
;
;BX:CX  - Starting linear address of memory to unlock
;SI:DI  - Size of region to unlock in bytes
;
;NOTES:
;
;This will allow the memory to be swapped to disk by the VMM if neccessary.
;Areas below and above the specified memory will also be un-locked if the
;specified region is not page aligned.
;
UnLockMem       equ     0ff1dh


;-------------------------------------------------------------------------------
;
;Un-lock a region of memory.
;
;On Entry:
;
;ESI    - Starting linear address of memory to unlock
;ECX    - Size of region to unlock in bytes
;
;NOTES:
;
;This will allow the memory to be swapped to disk by the VMM if neccessary.
;Areas below and above the specified memory will also be un-locked if the
;specified region is not page aligned.
;
UnLockMem32     equ     0ff1eh


;-------------------------------------------------------------------------------
;
;Lock a region of memory using application relative address.
;
;On Entry:
;
;ESI    - Starting linear address of memory to lock.
;EBX    - Size of region to lock in bytes.
;
;On Exit:
;
;Carry set on error, none of the memory locked, else memory is locked.
;
;NOTES:-
;
;Memory that is locked cannot be swapped to disk by the VMM. You should note
;that locking is applied to memory on 4k boundaries, so areas of memory below
;and above the memory being locked will also be locked if the specified region
;is not 4k aligned.
;
LockMemNear     equ     0ff1fh


;-------------------------------------------------------------------------------
;
;Un-lock a region of memory using application relative address.
;
;On Entry:
;
;ESI    - Starting linear address of memory to unlock
;EBX    - Size of region to unlock in bytes
;
;NOTES:
;
;This will allow the memory to be swapped to disk by the VMM if neccessary.
;Areas below and above the specified memory will also be un-locked if the
;specified region is not page aligned.
;
UnLockMemNear equ       0ff20h


;-------------------------------------------------------------------------------
;
;Allocate a region of DOS (conventional) memory.
;
;On Entry:
;
;BX     - Number of paragraphs (16 byte blocks) required.
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;
;AX     - Initial real mode segment of allocated block
;DX     - Initial selector for allocated block
;
;If function was not successful:
;Carry flag is set.
;
;AX     - DOS error code.
;BX     - Size of largest available block in paragraphs.
;
;NOTES:
;
;If the size of the block requested is greater than 64K bytes (BX > 1000h) then
;contiguous descriptors will be allocated. If more than one descriptor is
;allocated under 32-bit applications, the limit of the first descriptor will be
;set to the size of the entire block. All subsequent descriptors will have a
;limit of 64K except for the final descriptor which will have a limit of Block
;size MOD 64K. 16-bit applications will always set the limit of the first
;descriptor to 64K.
;
GetMemDOS       equ     0ff21h


;-------------------------------------------------------------------------------
;
;Re-size a block of DOS (conventional) memory previously allocated with
;GetMemDOS.
;
;On Entry:
;
;BX     - New block size in paragraphs
;DX     - Selector of block to modify
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;
;If function was not successful:
;Carry flag is set.
;
;AX     - DOS error code:
;BX     - Maximum block size possible in paragraphs
;
;NOTES:
;
;Growing a memory block is often likely to fail since other DOS block
;allocations will prevent increasing the size of the block. Also, if the size of
;a block grows past a 64K boundary then the allocation will fail if the next
;descriptor in the LDT is not free.
;
ResMemDOS       equ     0ff22h


;-------------------------------------------------------------------------------
;
;Release a block of DOS (conventional) memory previously allocated with
;GetMemDOS.
;
;On Entry:
;
;DX     - Selector of block to free.
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;
;If function was not successful:
;Carry flag is set.
;
;AX     - DOS error code.
;
;NOTES:
;
;All descriptors allocated for the memory block are automatically freed and
;therefore should not be accessed once the block is freed by this function.
;
RelMemDOS       equ     0ff23h


;-------------------------------------------------------------------------------
;
;Get current address and size of the buffer used for DOS memory transfers.
;
;On Exit:
;
;BX     - Real mode segment of buffer.
;DX     - Protected mode selector for buffer.
;ECX    - Buffer size.
;
;Notes:
;
;This buffer is used by the built in INT ?? API translation services,
;eg, INT 21h, AH=40h (write to file). The default buffer is 8k and uses memory
;that would otherwise be waisted. This default is sufficient for most file I/O
;but if you are writing a program that reads/writes large amounts of data you
;should consider allocateing your own larger buffer and pass the address to
;CauseWay to speed this file I/O.
;
GetDOSTrans     equ     0ff25h


;-------------------------------------------------------------------------------
;
;Set new address and size of the buffer used for DOS memory transfers.
;
;On Entry:
;
;BX     - Real mode segment of buffer.
;DX     - Protected mode selector for buffer.
;ECX    - Buffer size.
;
;Notes:
;
;This buffer is used by the built in INT ?? API translation services,
;eg, INT 21h, AH=40h (write to file). The default buffer is 8k and uses memory
;that would otherwise be waisted. This default is sufficient for most file I/O
;but if you are writing a program that reads/writes large amounts of data you
;should consider allocateing your own larger buffer and pass the address to
;CauseWay to speed this file I/O.
;
;The buffer must be in conventional memory and only the first 64k will be used
;even if a bigger buffer is specified. CauseWay will automaticaly restore the
;previous buffer setting when the application terminates but GetDOSTrans can
;be used to get the current buffer's settings if you only want the change to
;be temporary.
;
;You can still use the default buffer for your own puposes even after setting a
;new address.
;
SetDOSTrans     equ     0ff26h


;-------------------------------------------------------------------------------
;
;Get current MCB memory allocation block size.
;
;On Exit:
;
;ECX    - Current threshold.
;
GetMCBSize      equ     0ff27h


;-------------------------------------------------------------------------------
;
;Set new MCB memory allocation block size.
;
;On Entry:
;
;ECX    - New value to set.
;
;On Exit:
;
;Carry set on error else new value will be used.
;
;Notes:
;
;The maximum block size that will be allocated from MCB memory is 16 bytes less
;than the value set by this function. The default value is 16384.
;
;65536 is the maximum value this function will accept. Passing a value above
;this will return with the carry set and the original value still in force.
;
;The CauseWay API memory allocation functions allocate memory from two sources.
;Allocation requests below the value returned by this function are allocated
;from a memory pool controled via conventional style MCB's. Requests above this
;value are allocated via the normal DPMI functions. Because DPMI memory is
;always allocated in multiples of 4k it can become very inificient for any
;program that needs to allocate small blocks of memory. The value set by this
;function controls the size of memory chunks that will be allocated to and
;managed by the MCB system.
;
;A value of zero can be passed to this function to disable the MCB allocation
;system.
;
;The value passed will be rounded up to the nearest 4k.
;
SetMCBMax       equ     0ff28h


;-------------------------------------------------------------------------------
;
;Run another CauseWay program directly.
;
;On Entry:
;
;DS:EDX - File name.
;ES:ESI - Command line. First byte is length, then real data.
;CX     - Environment selector, 0 to use existing copy.
;
;On Exit:
;
;Carry set on error and AX = error code else AL=ErrorLevel
;
;Error codes:
;
;1      - DOS file access error.
;2      - Not a 3P file.
;3      - Not enough memory.
;
;NOTES:
;
;Only the first byte of the command line (length) has any significance to
;CauseWay so you are not restricted to ASCII values. It is still stored in the
;PSP at 80h though so the length is still limited to 127 bytes.
;
cwExec  equ     0ff24h


;-------------------------------------------------------------------------------
;
;Load another CauseWay program as an overlay, ie, do relocations etc but don't
;actually execute it.
;
;On Entry:
;
;DS:EDX - File name.
;
;On Exit:
;
;Carry set on error and AX = error code else,
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;
;Error codes:
;
;1      - DOS file access error.
;2      - Not a 3P file.
;3      - Not enough memory.
;
;NOTES:
;
;The PSP returned in SI can be passed to RelMem to release the loaded programs
;memory and selectors. Only the memory and selectors allocated during loading
;will be released, it is the programs responsability to release any additional
;memory etc allocated while the program is running. Alternatively, if you pass
;the PSP value to INT 21h, AH=50h before makeing additional memory requests
;and then reset to the original PSP the memory allocated will be released
;when the PSP is released.
;
cwLoad  equ     0ff2ah


;-------------------------------------------------------------------------------
;
;Validate and get expanded length of a CWC'd file.
;
;On Entry:
;
;BX     - File handle.
;
;On Exit:
;
;Carry set if not a CWC'd file else,
;
;ECX    - Expanded data size.
;
;NOTES:
;
;The file pointer is not altered by this function.
;
cwcInfo equ     0ff2bh


;-------------------------------------------------------------------------------
;
;Notify Debugger of new module loaded
;
;On Entry:
;
;EDI    - module PSP linear address (FLAT).
;
;On Exit:
;
;NOTES:
;
;
DbgNtfModLoad   equ     0fff7h


;-------------------------------------------------------------------------------
;
;Notify Debugger of module unloading
;
;On Entry:
;
;EDI    - module PSP linear address (FLAT).
;
;On Exit:
;
;NOTES:
;
;
DbgNtfModUnload equ     0fff8h


;-------------------------------------------------------------------------------
;
;Get CauseWay identifier, PageDIRLinear and Page1stLinear info.
;
;On Exit:
;
;ECX:EDX - CauseWay identifier.
;
;ESI     - Linear address (PageDIRLinear)
;
;EDI     - Linear address (Page1stLinear)
;
;NOTES:
;
;
ID equ          0fff9h


;-------------------------------------------------------------------------------
;
;Get patch table address.
;
;On Exit:
;
;EDX    - Linear address of patch table.
;
;Error codes:
;
;NOTES:
;
;
GetPatch equ    0fffah


;-------------------------------------------------------------------------------
;
;Load/Expand a CWC'd data file into memory.
;
;On Entry:
;
;BX     - Source file handle.
;ES:EDI - Destination memory.
;
;On Exit:
;
;Carry set on error and EAX is error code else,
;
;ECX    - Expanded data length.
;
;Error codes:
;
;       1 = Error during file access.
;       2 = Bad data.
;       3 = Not a CWC'd file.
;
;NOTES:
;
;The source file's file pointer doesn't have to be at zero. A single file might
;be several CWC'd files lumped together and as long as the file pointer is moved
;to the right place before calling this function.
;
;If error codes 1 or 2 are reported then the file pointer will be where ever it
;was last moved to by this function. For error code 3 the file pointer will be
;back at its original position on entry to this function. If no error occures
;then the file pointer will be moved to whatever comes after the compressed
;data.
;
cwcLoad     equ 0fffbh


;-------------------------------------------------------------------------------
;
;Check linear address of memory.
;
;On Entry:
;
;ESI    - Linear address of memory.
;
;On Exit:
;
;Carry set on invalid memory address.
;
;NOTES:
;
;
LinearCheck equ 0fffch


;-------------------------------------------------------------------------------
;
;Load CauseWay program for debug.
;
;On Entry:
;
;DS:EDX - File name.
;
;ES:ESI - Command line. First byte is length, then real data.
;
;CX     - Environment selector, 0 to use existing copy. 
;
;On Exit:
;
;Carry set on error and AX = error code else,
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;DI     - Auto DS.
;EBP    - Segment definition memory.
;
;Error codes:
;
;1      - DOS file access error.
;2      - Not recognisable file format.
;3      - Not enough memory.
;
;NOTES:
;
;
ExecDebug equ   0fffdh


;-------------------------------------------------------------------------------
;
;Close all open file handles.
;
;On Exit:
;
;NOTES:
;
;
CleanUp equ     0fffeh


;-------------------------------------------------------------------------------
;
;Structure of parameter table for real mode interupt and procedure calling.
;
;NOTE:- For interupts, CS:IP,SS:SP & Flags are filled in by the extender.
;       For far calls, SS:SP & Flags are filled in by the extender.
;
RealRegsStruc struc
Real_EDI        dd ?    ;EDI
Real_ESI        dd ?    ;ESI
Real_EBP        dd ?    ;EBP
        dd ?    ;Reserved.
Real_EBX        dd ?    ;EBX
Real_EDX        dd ?    ;EDX
Real_ECX        dd ?    ;ECX
Real_EAX        dd ?    ;EAX
Real_Flags      dw ?    ;FLAGS
Real_ES dw ?    ;ES
Real_DS dw ?    ;DS
Real_FS dw ?    ;FS
Real_GS dw ?    ;GS
Real_IP dw ?    ;IP
Real_CS dw ?    ;CS
Real_SP dw ?    ;SP
Real_SS dw ?    ;SS
RealRegsStruc ends


;-------------------------------------------------------------------------------
;
;Now the extended PSP structure.
;
EPSP_Struc              struc
                    db 256 dup (?)
 EPSP_Parent        dw ?            ;Selector of parent/previous PSP, 0 for none.
 EPSP_Next          dw ?            ;Next PSP.
 EPSP_Resource      dd ?            ;Linear address of resource tracking table. 0 for none.
 EPSP_mcbHead       dd ?            ;Linear address of MCB memory head. 0 for none.
 EPSP_mcbMaxAlloc   dd ?            ;Size of MCB chunks.
 EPSP_DTA           df ?            ;DTA address.
 EPSP_TransProt     dw ?            ;Transfer buffer address.
 EPSP_TransReal     dw ?            ;Transfer buffer real mode segment value.
 EPSP_TransSize     dd ?            ;Transfer buffer size.
 EPSP_SSESP         df ?            ;Return SS:ESP to use.
 EPSP_INTMem        dd ?            ;linear address of interrupt/exception vector save buffer. 0 for none.
 EPSP_DPMIMem       dw ?            ;selector for DPMI state save buffer. 0 for none.
 EPSP_MemBase       dd ?            ;Program linear load address.
 EPSP_MemSize       dd ?            ;Program memory size.
 EPSP_SegBase       dw ?            ;Base program selector.
 EPSP_SegSize       dw ?            ;Number of program selectors.

 EPSP_NearBase      dd ?            ;NEAR function address translation base.

 EPSP_RealENV       dw ?            ;Origional real mode environment SEGMENT.

 EPSP_NextPSP       dd ?            ;Pointer to next PSP
 EPSP_LastPSP       dd ?            ;Pointer to last PSP

 EPSP_Exports       dd ?            ;Pointer to export list.
 EPSP_Imports       dd ?            ;Pointer to import list.

 EPSP_Links         dd ?            ;Count of linked modules.

 EPSP_ExecCount     dd ?            ;PMode pointer to exec counter.

 EPSP_EntryCSEIP    df ?            ;Entry CS:EIP for DLL's

 EPSP_PSPSel        dw ?            ;This PSP's selector.

 EPSP_FileName      db 256 dup (?)  ;This module full file name.

EPSP_Struc              ends


