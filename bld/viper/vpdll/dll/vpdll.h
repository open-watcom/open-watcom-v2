/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>
#define VPDLL_VERSION           1

#define DLL_EXPORT    __export __pascal

// Messages for the Callback Function

#define VPDLL_GEN_CODE          1
#define VPDLL_NEW_PROJECT       2
#define VPDLL_SHUT_DOWN         3
#define VPDLL_TO_FRONT          4
#define VPDLL_PROJ_DIRTY        5
#define VPDLL_SRC_DIRTY         6

/****************************************************************************
 * Function:    VPDLL_GetVersion
 * Notes:       This function should be called by Visual Programmer
 *              immediately after the DLL is loaded to verify that the
 *              correct DLL has been loaded.  If the returned value is not
 *              VPDLL_VERSION then Visual Programmer must make no other call
 *              into the DLL.
 * Return:      The function returns VPDLL_VERSION.
 ***************************************************************************/
DWORD DLL_EXPORT VPDLL_GetVersion( void );

/***************************************************************************
 * Function:    VPDLL_Init
 * Notes:       This function should be called immediately after calling
 *              VPDLL_GetVersion and successfully verifying that the correct
 *              version of the DLL has been loaded.  This function causes the
 *              DLL to initialize itself.  A callback function must be
 *              registered by this call.  This callback must be able to
 *              perform certain tasks on behalf of the IDE.  See the section
 *              on Call-back Transactions at the end of this file for
 *              information on these requests.
 * Parameters:  fn - a FARPROC (procedure-instance address) for the callback
 *                   function.
 *                   The function should be exported and declared PASCAL.  The
 *                   pointer passed in must be one returned by a call to
 *                   MakeProcInstance.
 * Return:      The function returns TRUE when an error occurs or FALSE
 *              otherwise.  When TRUE is returned Visual Programmer should
 *              not make make any other calls into the DLL.
 ***************************************************************************/
BOOL DLL_EXPORT VPDLL_Init( BOOL PASCAL (*fn)( DWORD msg, DWORD parm1, DWORD parm2 ) );

/***************************************************************************
 * Function:    VPDLL_ReturnToIDE
 * Notes:       Calling this function will cause the IDE to bring itself
 *              to the front.  Visual Programmer should call this when the
 *              user selects the WATCOM IDE button.
 ***************************************************************************/
void DLL_EXPORT VPDLL_ReturnToIDE( void );

/***************************************************************************
 * Function:    VPDLL_BeginFileList
 * Notes:       VPDLL_BeginFileList, VPDLL_AddFile and VPDLL_EndFileList
 *              are used together to allow Visual Programmer to tell the IDE
 *              what files are currently part of the project.  Whenever it
 *              wants to send an updated file list to the IDE, Visual
 *              Programmer will call VPDLL_BeginFileList. It then calls
 *              VPDLL_AddFile once for each source file (eg. .c .cpp .rc .dlg
 *              .bmp etc) in the project.  Finally, VPDLL_EndFileList should
 *              be called.  When this call is made the IDE will update its
 *              file list based on the previous calls to VPDLL_AddFile.  Any
 *              files previously added by Visual Programmer but not included
 *              in the new list will be removed and any new files will be
 *              added.  Visual Programmer should enumerate the file list
 *              immediately after it loads a new project and immediately after
 *              it generates code.
 * Return:      The function returns TRUE when an error occurs or FALSE
 *              otherwise.
 *              If TRUE is returned, display an error message and do not
 *              attempt to call VPDLL_AddFile or VPDLL_EndFileList.
 ***************************************************************************/
BOOL DLL_EXPORT VPDLL_BeginFileList( void );

/****************************************************************************
 * Function:    VPDLL_AddFile
 * Notes:       See VPDLL_BeginFileList
 * Parameters:  fname - This should be the fully qualified path name of the
 *                      added file (eg. d:\dev\foo.cpp).
 * Return:      The function returns TRUE when an error occurs or FALSE
 *              otherwise.
 *              If TRUE is returned, display an error message and do not
 *              continue to call VPDLL_AddFile.  Also, do not call
 *              VPDLL_EndFileList.
 ***************************************************************************/
BOOL DLL_EXPORT VPDLL_AddFile( char *fname );

/****************************************************************************
 * Function:    VPDLL_EndFileList
 * Notes:       See VPDLL_BeginFileList
 * Return:      The function returns TRUE when an error occurs or FALSE
 *              otherwise.
 *              If TRUE is returned, display an error message.
 ***************************************************************************/
BOOL DLL_EXPORT VPDLL_EndFileList( void );

/***************************************************************************
 * Function:    VPDLL_VPDone
 * Notes:       Visual Programmer should call this function immediately
 *              before it shuts down.
 **************************************************************************/
void DLL_EXPORT VPDLL_VPDone( void );

/***************************************************************************
BOOL PASCAL callback( DWORD msg, DWORD parm1, DWORD parm2 );
 * Callback Transactions
 *      The function registered by the VPDLL_Init call will be called
 *      from time to time to perform services.  The service to be performed
 *      is determined by the value of the msg parameter as follows:
 *
 * VPDLL_GEN_CODE       - When Visual Programmer gets this message, it should
 *                        generate new source code to the disk if needed.  If
 *                        parm1 is non-zero, Visual Programmer should
 *                        query the user before writing any new code to the
 *                        disk.  Visual Programmer should not return from
 *                        processing this message until the new code has been
 *                        generated and a new file list has been given to the
 *                        IDE using the BeginFileList, AddFile, EndFileList
 *                        interface.  When processing the message return FALSE
 *                        if new code was successfully generated and a new
 *                        file list given to the IDE or TRUE otherwise.
 * VPDLL_NEW_PROJECT    - When Visual Programmer gets this message, parm1
 *                        contains a far pointer to the name of a new
 *                        Visual Programmer project file.  When it gets this
 *                        messsage Visual Programmer should give the user the
 *                        chance to save the current project (if needed), then
 *                        it should load the specified project file if it
 *                        exists.  If it does not exist, Visual Programmer
 *                        should create a new project with that name and let
 *                        the user pick a project template.  When processing
 *                        this message, return FALSE if the new project was
 *                        successfully loaded or created.  Return TRUE
 *                        otherwise.
 * VPDLL_SHUT_DOWN      - When Visual Programmer gets this message, it should
 *                        shut down after offering the user the chance to save
 *                        changes (as it would if the user chose exit from the
 *                        file menu).  The value returned when this message is
 *                        sent is not used.
 * VPDLL_TO_FRONT       - When Visual Programmer gets this message, it should
 *                        bring its main window to the front of the Windows
 *                        desktop. The value returned when this message
 *                        is sent is not used.
 * VPDLL_PROJ_DIRTY     - When it receives this message, Visual Programmer
 *                        should return TRUE if the project has changed since
 *                        it was last saved or FALSE otherwise.
 * VPDLL_SRC_DIRTY      - When it receives this message, Visual Programmer
 *                        should return TRUE if the project has changed since
 *                        its source files were last generated.
 *
 * Except where noted, 'parm1' and 'parm2' are not used.
 ***************************************************************************/
