	.386
	.model small
	option oldstructs
	option proc:private


	include equmac.inc
	include c.inc


;******************************************************************************
;Run specified program with command line. Passes statement to command.com.
;
;Usage: Run(statement);
;
;statement	- ASCII statement to pass to command.com
;
;Returns:
;
;AL	- DOS exec error code.
;AH	- errorlevel if AL == 0.
;
;******************************************************************************
	externdef run:near


;*******************************************************************************
;Allocate some locked memory.
;
;Usage: MallocL(bytes);
;
;bytes	- Bytes required.
;
;Returns:
;
;EAX	- Address of allocated memory, zero on error.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef mallocl:near


;*******************************************************************************
;Free some locked memory.
;
;Usage: FreeL(void *memory);
;
;memory	- Address to release.
;
;On Exit:
;
;EAX	- Address released if ok, zero on error.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef freel:near


;*******************************************************************************
;Lock a memory region.
;
;Usage: MemLock(void *memory, int bytes);
;
;memory	- address to lock
;bytes	- bytes to lock.
;
;Returns:
;
;EAX	- bytes locked. (basicly bytes or zero)
;
;*******************************************************************************
	externdef memlock:near


;*******************************************************************************
;Unlock a memory region.
;
;Usage: MemUnLock(void *memory, int bytes);
;
;memory	- address to unlock.
;bytes	- bytes to unlock.
;
;Returns:
;
;EAX	- bytes un-locked.
;
;*******************************************************************************
	externdef memunlock:near


;*******************************************************************************
;
;Allocate some conventional memory.
;
;Usage: mallocdos(bytes);
;
;bytes	- bytes to allocate.
;
;On Exit:
;
;EAX	- memory allocated, zero if failed.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef mallocdos:near


;*******************************************************************************
;
;Re-allocate some memory allocated with "MallocDOS".
;
;Usage: reallocdos(old_mem, bytes);
;
;old_mem	- existing memory address.
;bytes	- new size in bytes.
;
;Returns:
;
;EAX	- memory allocated, zero if failed.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef reallocdos:near


;*******************************************************************************
;
;Free some memory allocated with "MallocDOS".
;
;Usage: freedos(memory);
;
;memory	- address of allocated memory.
;
;Returns:
;
;All registers preserved.
;
;*******************************************************************************
	externdef freedos:near


;******************************************************************************
;Allocate memory for, and load, a file.
;
;Usage: LoadAlloc(filename);
;
;filename - name of file to load.
;
;Returns:
;
;EAX	- error code if < 4 else load address.
;	0 - file not found
;	1 - file read error.
;	2 - not enough memory.
;
;All other registers preseved.
;
;******************************************************************************
	externdef LoadAlloc:near


;******************************************************************************
;Allocate locked memory for, and load, a file. Copes with CWC compresion.
;
;Usage: LoadAllocL(filename);
;
;filename - name of file to load.
;
;Returns:
;
;EAX	- error code if < 4 else load address.
;	0 - file not found
;	1 - file read error.
;	2 - not enough memory.
;
;All other registers preseved.
;
;******************************************************************************
	externdef LoadAllocL:near


;*******************************************************************************
;
;Open a file and setup buffering for it.
;
;Usage: OpenFile(filename,flags);
;
;filename	- pointer to zero terminated ASCII file name.
;flags	- flags.
;
;Returns:
;
;EAX	- file access handle.
;
;If EAX=0 then the open failed and FIOError has the error code.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef OpenFile:near
	externdef REDOpenFile:dword


;*******************************************************************************
;
;Create a file and setup buffering for it.
;
;Usage: CreateFile(filename,flags);
;
;filename	- pointer to zero terminated ASCII file name.
;flags	- flags.
;
;Returns:
;
;EAX	- file access handle.
;
;If EAX=0 then the create failed and FIOError has the error code.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef CreateFile:near


;*******************************************************************************
;
;Close a file opened with "OpenFile" or "CreateFile". Any dirty data in the
;buffer will be flushed first.
;
;Usage: CloseFile(handle);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;
;Returns:
;
;EAX	- error code, zero for none. FIOError is also updated.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef CloseFile:near


;*******************************************************************************
;
;Set file pointer within a file opened with "OpenFile" or "CreateFile".
;
;Usage: SeekFile(handle,offset,mode);
;
;mode	- mode (same as DOS)
;handle	- file handle returned by "OpenFile" or "CreateFile"
;offset	- new pointer value to set.
;
;Returns:
;
;EAX	- absolute offset or error detected if -1.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef SeekFile:near


;*******************************************************************************
;
;Read data from a file opened with "OpenFile" or "CreateFile".
;
;Usage: ReadFile(handle,buffer,bytes);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;buffer	- address to transfer data to.
;bytes	- number of bytes to read.
;
;Returns:
;
;EAX	- bytes transfered, -1 for error & FIOError has error code.
;
;*******************************************************************************
	externdef ReadFile:near


;*******************************************************************************
;
;Read an ASCII line of data from a file.
;
;Usage: ReadLineFile(handle,buffer,maxlen);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;buffer	- buffer to put the line in.
;maxlen	- maximum line length to allow.
;
;Returns:
;
;EAX	- status.
;	0=line is in buffer.
;	1=EOF, nothing in the buffer.
;	2=line length overun.
;	3=read error.
;	4=CR/LF pair mismatch.
;
;A zero terminated string with CR, LF and soft EOF's removed is read into the
;buffer specified. The removal of CR & LF and soft EOF's are the only
;modifications applied to the data read.
;
;Notes:
;
;EOF is never returned with data in the buffer. Even if EOF is encountered at
;the end of a line with no CR/LF that line will be returned as normal. The next
;use of this function will return EOF.
;
;Line length overrun returns a valid buffer (zero terminated) and the file
;pointer is moved to the next line ready for the next line read. It's upto the
;caller what to do about this error but this function will continue to work
;correctly after this error has been returned.
;
;A read error may be returned with some data in the buffer but generaly that
;data should be regarded as unreliable. Further use of this function should be
;avoided after a read error is reported.
;
;A CR/LF pair mismatch is reported when a CR is read and the next byte isn't an
;LF. The consequences are the same as for a read error.
;
;*******************************************************************************
	externdef ReadLineFile:near


;*******************************************************************************
;
;Read a byte from a file opened with "OpenFile" or "CreateFile".
;
;Usage: ReadByteFile(handle);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;
;Returns:
;
;EAX	- byte read or -1 for error, FIOError is error code.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef ReadByteFile:near


;*******************************************************************************
;
;Write data to a file opened with "OpenFile" or "CreateFile".
;
;Usage: WriteFile(handle,buffer,bytes);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;buffer	- address to transfer data from.
;bytes	- number of bytes to write.
;
;Returns:
;
;EAX	- bytes written or -1 on error, FIOError has error code.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef WriteFile:near


;*******************************************************************************
;
;Write a byte to a file opened with "OpenFile" or "CreateFile".
;
;Usage: WriteByteFile(handle,byte);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;byte	- byte to write.
;
;Returns:
;
;EAX	- byte written, -1 for error & FIOError has error code.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef WriteByteFile:near


;*******************************************************************************
;
;Flush any dirty buffered data for a file opened with "OpenFile" or "CreateFile"
;to disk.
;
;Usage: FlushFile(handle);
;
;handle	- file handle returned by "OpenFile" or "CreateFile"
;
;Returns:
;
;EAX	- error code, zero for no error.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef FlushFile:near


;*******************************************************************************
;
;Buffer size and error code storage to use with above functions.
;
;*******************************************************************************
	externdef FIOBufferSize:dword
	externdef FIOError:dword


FIO_struc	struc
FIO_Flags	dd ?		;Control flags.
FIO_Flags_DOS	equ 1		;Buffer is in conventional memory.
FIO_Flags_Dirty equ 2	;Buffer is dirty.
FIO_Handle	dd ?		;File's DOS handle.
FIO_Pointer	dd ?		;Current file pointer position.
FIO_BPointer	dd ?		;Buffer file pointer position.
FIO_Buffered	dd ?		;Number of bytes in the buffer.
FIO_Length	dd ?		;Length of the buffer.
FIO_Spare	dd 2 dup (?)	;reserved.
FIO_struc	ends


;*******************************************************************************
;Copy a file to specified destination.
;
;Usage: CopyFile(source,dest);
;
;source	- source file name.
;dest	- destination file name.
;
;Returns:
;
;EAX	- bytes copied, -1 on error.
;
;*******************************************************************************
	externdef CopyFile:near


;*******************************************************************************
;Build a list of all valid drives on the system.
;
;Usage: GetDrives(char *buffer);
;
;buffer	- Address to build list of drives.
;
;On Exit:
;
;EAX	- number of drives.
;
;ALL other registers preserved.
;
;The drive list uses real drive codes (not ASCII). Each entry uses 1 byte
;and the list is terminated with -1.
;
;*******************************************************************************
	externdef GetDrives:near


;******************************************************************************
;
;Get the current drive.
;
;Usage: GetDrive();
;
;Returns:
;
;EAX	- Drive code.
;
;******************************************************************************
	externdef GetDrive:near


;*******************************************************************************
;Get current drive and path.
;
;Usage: GetPath(char *buffer);
;
;buffer	- pointer to buffer.
;
;On Exit:
;
;ALL registers preserved.
;
;*******************************************************************************
	externdef GetPath:near


;******************************************************************************
;
;Set the current drive.
;
;Usage: SetDrive(drive);
;
;drive	- drive code to set.
;
;Returns:
;
;ALL registers preserved.
;
;******************************************************************************
	externdef SetDrive:near


;*******************************************************************************
;Set current drive and path.
;
;Usage: SetPath(char *buffer);
;
;buffer	- pointer to path string.
;
;On Exit:
;
;EAX	- error code.
;0	- no error.
;
;ALL other registers preserved.
;
;*******************************************************************************
	externdef SetPath:near


;*******************************************************************************
;Get key press info.
;
;Usage: GetKey();
;
;none.
;
;On Exit:
;
;AL	- ASCII character code.
;AH	- Hardware key code.
;EAX high word	- Shift keys status word from 417h.
;
;*******************************************************************************
	externdef GetKey:near
SK_SHIFTR	equ	1
SK_SHIFTL	equ	2
SK_CTRL	equ	4
SK_ALT	equ	8
SK_SCROLL	equ	16
SK_NUM	equ	32
SK_CAPS	equ	64
SK_INS	equ	128


;******************************************************************************
;
;Convert a string to upper case.
;
;Usage: strupr(string);
;
;Returns:
;
;All registers preserved.
;
;******************************************************************************
	externdef strupr:near


;*******************************************************************************
;Print null terminated string on screen via DOS.
;
;Usage: PrintString(char *buffer);
;
;buffer	- pointer to string to print.
;
;*******************************************************************************
	externdef PrintString:near


;*******************************************************************************
;Convert number into decimal ASCII.
;
;Usage: Bin2Dec(int num, char *buffer);
;
;num	- Number to convert.
;buffer	- Buffer to put it in.
;
;On Exit:
;
;EAX	- Next available byte in buffer.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef Bin2Dec:near


;*******************************************************************************
;Convert decimal zero terminated ASCII string into binary value.
;
;Usage: Dec2Bin(char *buffer, int *result);
;
;buffer	- pointer to source string
;result	- int to recieve the result.
;
;Returns:
;
;EAX	- status, 0=ok, none-zero=invalid character.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef Dec2Bin:near


;*******************************************************************************
;Convert number into ASCII Hex version.
;
;Usage: Bin2Hex(int num, int digits, char *buffer);
;
;num	- Number to convert.
;digits	- Digits to do.
;buffer	- Buffer to put string in.
;
;On Exit:
;
;EDI	- Next un-used byte in buffer.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef Bin2Hex:near


;*******************************************************************************
;Convert numerical zero terminated expresion into binary value.
;
;Usage: ASCII2Bin(char *buffer, int *result);
;
;buffer	- pointer to source string
;result	- int to recieve the result.
;
;Returns:
;
;EAX	- status, 0=ok, none-zero=invalid character.
;
;All other registers preserved.
;
;*******************************************************************************
	externdef ASCII2Bin:near


;*******************************************************************************
;Read the command line tail for parameters.
;
;supports / or - or + as switch/option settings. Options and names may be in
;any order, text can be tagged onto options, names must be in correct sequence
;for current program.
;
;Each character ( 33 to 127) has an entry in OptionTable & OptionTable+128.
;
;The first entry is a byte, and is 0 for OFF & none-zero for ON.
;The second entry is a pointer to any additional text specified, 0 means none.
;Entries 0-32 are reserved for text not preceded by - or + or /. These are
;intended to be file names, but neadn't be.
;All text entries are 0 terminated.
;
;OptionCounter	 - Total command line parameters (files & switches).
;OptionTable	 - Switch table, ASCII code is index to check.
;OptionPointers - Pointer table, ASCII code*4 is index to use.
;
;Usage: ReadCommand();
;
;Returns:
;
;EAX	- number of none-switch strings (file names).
;
;*******************************************************************************
	externdef ReadCommand:near
	externdef OptionCounter:byte
	externdef OptionTable:byte
	externdef OptionPointers:dword


;*******************************************************************************
;
;Return the true/false value of a config file option.
;
;Usage: cfgGetOnOff(filename,section,variable,result);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;result	- address to put the result.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, result updated.
;	1=file not found.
;	2=section not found.
;	3=option not found.
;	4=file I/O error occured.
;	5=invalid option value.
;
;The option can be: ON or OFF, 1 or 0, YES or NO, TRUE or FALSE
;ON,0,YES & TRUE are returned as none-zero, OFF,0,NO and FALSE as zero.
;
;*******************************************************************************
	externdef cfgGetOnOff:near


;*******************************************************************************
;
;Return the integer value of a config file option.
;
;Usage: cfgGetInt(filename,section,variable,result);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;result	- address to put the result.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, result updated.
;	1=file not found.
;	2=section not found.
;	3=option not found.
;	4=file I/O error occured.
;	5=invalid option value.
;
;*******************************************************************************
	externdef cfgGetInt:near


;*******************************************************************************
;
;Set the integer value of a config file option.
;
;Usage: cfgSetInt(filename,section,variable,value);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;value	- value to write.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, value updated.
;	1=file not found.
;	2=section not found.
;	4=file I/O error occured.
;	5=invalid option value.
;
;*******************************************************************************
	externdef cfgSetInt:near


;*******************************************************************************
;
;Return the integer list of values of a config file option.
;
;Usage: cfgGetInts(filename,section,variable,results,maxres);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;results	- address to put the results.
;maxres	- maximum number of results.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, result updated.
;	1=file not found.
;	2=section not found.
;	3=option not found.
;	4=file I/O error occured.
;	5=invalid option value.
;	6=maxres reached with more values to come.
;
;*******************************************************************************
	externdef cfgGetInts:near


;*******************************************************************************
;
;Return the string assigned to a config file option.
;
;Usage: cfgGetString(filename,section,variable,result,maxlen);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;result	- address to put the result.
;maxlen	- maximum string length.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, result updated.
;	1=file not found.
;	2=section not found.
;	3=option not found.
;	4=file I/O error occured.
;	5=invalid option value.
;	6=maxlen exceded.
;
;*******************************************************************************
	externdef cfgGetString:near


;*******************************************************************************
;
;Set the string value of a config file option.
;
;Usage: cfgSetString(filename,section,variable,value);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;value	- address of string to write.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, value updated.
;	1=file not found.
;	2=section not found.
;	4=file I/O error occured.
;	5=invalid option value.
;
;*******************************************************************************
	externdef cfgSetString:near


;*******************************************************************************
;
;Delete a config file option.
;
;Usage: cfgDelOption(filename,section,variable);
;
;filename	- name of the config file.
;section	- section name.
;variable	- variable/option text.
;
;Returns:
;
;EAX	- status code.
;	0=all ok, value updated.
;	1=file not found.
;	4=file I/O error occured.
;
;*******************************************************************************
	externdef cfgDelOption:near


;*******************************************************************************
;
;Find a config file option within a [section]
;
;Usage: cfgFindSectionOption(handle,section,variable);
;
;handle	- file handle to search.
;section	- section name.
;variable	- variable/option text.
;
;Returns:
;
;EAX	- status code.
;	0=option found.
;	1=reserved.
;	2=section not found.
;	3=option not found.
;	4=file I/O error occured.
;
;The file is searched from the current position so it's up to the caller to
;make sure it starts in the right place.
;
;*******************************************************************************
	externdef cfgFindSectionOption:near


;*******************************************************************************
;
;Find a config file [section]
;
;Usage: cfgFindSection(handle,section);
;
;handle	- file handle to search.
;section	- section name to search for.
;
;Returns:
;
;EAX	- status code.
;	0=section found.
;	1=section NOT found.
;	2=file I/O error.
;
;The file is searched from the current position so it's up to the caller to
;make sure it starts in the right place.
;
;*******************************************************************************
	externdef cfgFindSection:near


;*******************************************************************************
;
;Find an option within current section.
;
;Usage: cfgFindOption(handle,variable);
;
;handle	- file handle to search.
;variable	- option text to find.
;
;Returns:
;
;EAX	- status code.
;	0=option found.
;	1=option NOT found.
;	2=file I/O error.
;
;The file is searched from the current position so it's up to the caller to
;make sure it starts in the right place. A line with "[" as the first character
;or EOF will be treated as the end of the section.
;
;*******************************************************************************
	externdef cfgFindOption:near
	externdef cfgIOBuffer:byte


;*******************************************************************************
;Generate list of files that match a given file mask. Wild cards permitted.
;
;Usage: MakeFileList(char *filename, int attribs);
;
;filename	- File name mask (can include a path).
;attribs	- search attributes.
;
;On Exit:
;
;EAX	- buffer with file names etc, EAX=0 if not enough memory.
;
;Each entry in the buffer will have the following format.
;
;13 bytes zero terminated ASCII file name (may be less)
; 1 byte file attribute (directory,vulume,hidden,system etc.)
;
; If you specify directories in the attribute search mask the names . & .. will
; be removed from the list to avoid making special checks for them. Anything
; that might have found these useful will have to have knowledge of its
; current path anyway.
;
;
;Attributes are as follows:-
;
;  5  4  3  2  1  0
;  |  |  |  |  |  |
;  |  |  |  |  |  \_ Read only. (1)
;  |  |  |  |  \____ Hidden.    (2)
;  |  |  |  \_______ System.    (4)
;  |  |  \__________ Label.     (8)
;  |  \_____________ Directory. (16)
;  \________________ Archive.   (32)
;
; Bits 6-15 are reserved and may be used in the search mask, but bits 8-15 won't
; appear in the attributes for listed files.
;
; The first dword in the buffer is the number of entries.
; Next 256 bytes is the path to append the names to.
;
;*******************************************************************************
	externdef MakeFilelist:near


;*******************************************************************************
;Make a full drive:path\file name from a file list path string and specified
;file name.
;
;Usage: FilelistMakeName(int entry, void *filelist, void *buffer);
;
;entry	- entry number to make name from.
;filelist	- pointer to file list.
;buffer	- pointer to destination buffer.
;
;Returns:
;
;ALL registers preserved.
;
;Name in a file list are just the 8.3 portion. A more useful name can be made
;by combineing the path string from the start of the list buffer with the file
;name which this routine does.
;
;*******************************************************************************
	externdef FilelistMakeName:near


;*******************************************************************************
;Convert normal names to lower case and directory names to upper case.
;
;Usage: ULFileList(void *filelist);
;
;filelist	- pointer to file list.
;
;On Exit:
;
;ALL registers preserved.
;
;Most routines that need to display a file list want directory names in upper
;case and the remaining names in lower case. This routine formats a file list
;to that specification. Doing things this way means that if the file list is
;then sorted, directory names will appear at the top.
;
;*******************************************************************************
	externdef ULFilelist:near


;*******************************************************************************
;Sort a file list into alpha-numeric order. Assumes you want DIR's at the top
;of the list.
;
;Usage: SortFileList(void *filelist);
;
;filelist	- pointer to file list to sort.
;
;On Exit:
;
;ALL registers preserved.
;
;File names are not case converted so upper case names will appear at the top
;of the list.
;
;*******************************************************************************
	externdef SortFilelist:near


;*******************************************************************************
;Wait for a while.
;
;Usage: Delay(int period);
;
;Returns:
;
;nothing.
;
;*******************************************************************************
	externdef Delay:near


;*******************************************************************************
;This gets used in variables that need a valid destination that doesn't actually
;do anything. Saves keep checking for zero before makeing a CALL or JMP.
;*******************************************************************************
	externdef DummyRET:near



