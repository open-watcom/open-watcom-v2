.****************************************************************************
.*
.*                            Open Watcom Project
.*
.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
.*
.*  ========================================================================
.*
.*    This file contains Original Code and/or Modifications of Original
.*    Code as defined in and that are subject to the Sybase Open Watcom
.*    Public License version 1.0 (the 'License'). You may not use this file
.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
.*    provided with the Original Code and Modifications, and is also
.*    available at www.sybase.com/developer/opensource.
.*
.*    The Original Code and all software distributed under the License are
.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
.*    NON-INFRINGEMENT. Please see the License for the specific language
.*    governing rights and limitations under the License.
.*
.*  ========================================================================
.*
.* Description:  Error Codes.
.*
.* Date         By              Reason
.* ----         --              ------
.* 07-aug-92    Craig Eisler    initial draft
.* 03-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=errcode 'Error Code Tokens'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
These are the tokens defined in the file :fname.error.dat:efname.
that you can use to identify different errors in an editor script.
A typical usage would be:

.millust begin
if lastrc != ERR_NO_ERR
    ... handle error ...
else
    ... no error ...
endif
.millust end

.*
:DL break.
.*
:DT.END_OF_FILE
:DD.Returned if a fread command is done at the end of file.

:DT.ERR_DIRECTORY_OP_FAILED
:DD.Returned if the last cd command was to a non-existent directory.

:DT.ERR_FILE_EXISTS
:DD.Returned if the last write command tried to overwrite an existing file.

:DT.ERR_FILE_MODIFIED
:DD.Returned if you attempt to quit a modified file.

:DT.ERR_FILE_NOT_FOUND
:DD.Returned by the read command or the fopen command if the file could not be found.

:DT.ERR_FILE_VIEW_ONLY
:DD.Returned if you attempt to modify a view only file.

:DT.ERR_FIND_END_OF_FILE
:DD.Returned if nosearchwrap is set, and the last search command encountered the end of the edit buffer.

:DT.ERR_FIND_NOT_FOUND
:DD.Returned if the last search command didn't find the string.

:DT.ERR_FIND_TOP_OF_FILE
:DD.Returned if nosearchwrap is set, and the last search command encountered the top of the edit buffer.

:DT.ERR_INVALID_COMMAND
:DD.Returned if the last command was invalid.

:DT.ERR_INVALID_SET_COMMAND
:DD.Returned if the last set command was an invalid one.

:DT.ERR_NO_ERR
:DD.Returned if the last operation was a success.

:DT.ERR_NO_FILE_NAME
:DD.Returned if you attempt to write a file with no file name.

:DT.ERR_NO_MORE_UNDOS
:DD.Returned if the last undo command didn't undo anything.

:DT.ERR_NO_SUCH_DRIVE
:DD.Returned if the last cd command was to a non-existent drive.

:DT.ERR_NO_SUCH_LINE
:DD.Returned if the last movement command went to an invalid line.

:DT.ERR_READ_ONLY_FILE
:DD.Returned if you attempt to modify a read only file.

:DT.NEW_FILE
:DD.Value of lastrc in a read hook script, if the file just edited was a new file.

:DT.NO_VALUE_ENTERED
:DD.Returned if an input command was cancelled by the user.

:eDL.
