.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 07-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id=errcode.Error Code Tokens
These are the tokens defined in the file
:KEYWORD.error.dat
that you can use to identify different errors in an editor script.
A typical usage would be:
:ILLUST.
if lastrc != ERR_NO_ERR
    ... handle error ...
else
    ... no error ...
endif
:eILLUST.
:P.
:DEFLIST.
:DEFITEM.END_OF_FILE
Returned if a fread command is done at the end of file.

:DEFITEM.ERR_DIRECTORY_OP_FAILED
Returned if the last cd command was to a non-existent directory.

:DEFITEM.ERR_FILE_EXISTS
Returned if the last write command tried to overwrite an existing file.

:DEFITEM.ERR_FILE_MODIFIED
Returned if you attempt to quit a modified file.

:DEFITEM.ERR_FILE_NOT_FOUND
Returned by the read command or the fopen command if the file could not be found.

:DEFITEM.ERR_FILE_VIEW_ONLY
Returned if you attempt to modify a view only file.

:DEFITEM.ERR_FIND_END_OF_FILE
Returned if nosearchwrap is set, and the last search command encountered the end of the edit buffer.

:DEFITEM.ERR_FIND_NOT_FOUND
Returned if the last search command didn't find the string.

:DEFITEM.ERR_FIND_TOP_OF_FILE
Returned if nosearchwrap is set, and the last search command encountered the top of the edit buffer.

:DEFITEM.ERR_INVALID_COMMAND
Returned if the last command was invalid.

:DEFITEM.ERR_INVALID_SET_COMMAND
Returned if the last set command was an invalid one.

:DEFITEM.ERR_NO_ERR
Returned if the last operation was a success.

:DEFITEM.ERR_NO_FILE_NAME
Returned if you attempt to write a file with no file name.

:DEFITEM.ERR_NO_MORE_UNDOS
Returned if the last undo command didn't undo anything.

:DEFITEM.ERR_NO_SUCH_DRIVE
Returned if the last cd command was to a non-existent drive.

:DEFITEM.ERR_NO_SUCH_LINE
Returned if the last movement command went to an invalid line.

:DEFITEM.ERR_READ_ONLY_FILE
Returned if you attempt to modify a read only file.

:DEFITEM.NEW_FILE
Value of lastrc in a read hook script, if the file just edited was a new file.

:DEFITEM.NO_VALUE_ENTERED
Returned if an input command was cancelled by the user.

:eDEFLIST.
