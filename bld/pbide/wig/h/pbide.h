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


#ifndef PBIDE_H_INCLUDED
#define PBIDE_H_INCLUDED

#define WAT_IDE_DLL_CUR_VER     1
#ifdef __NT__
#pragma aux __fortran "*" parm [] modify [ eax ecx edx ];
#define IDE_EXPORT              __export __fortran
#else
#define IDE_EXPORT              __export __pascal
#endif

typedef enum {
    FN_GET_CONSTRUCTOR,
    FN_GET_DESTRUCTOR,
    FN_GET_FUNCTION
} FunctionRequest;

typedef struct {
    unsigned            errcnt;         /* number of errors encountered */
    unsigned            warncnt;        /* number of warnings encountered */
    char                errors[1];      /* dynamic array containing the
                                           error and warning messages */
} WatIDEErrInfo;

/* WatIDE_GetVersion - This function can be used to ensure that you have
 *              the correct version of the DLL loaded. It should be called
 *              immediately after the DLL is loaded.
 *
 *      Returns: The version of this DLL. If the returned value is not
 *              WAT_IDE_DLL_CUR_VER then do not use the DLL.
 */
unsigned IDE_EXPORT WatIDE_GetVersion( void );

/* WatIDE_DirIsOK - This function does a simple check to see if a directory
 *              contains files that may interfere with building a new DLL
 *              in that directory.  This check is not fool proof!!!!
 *
 *      Parameters
 *              dllname - this should point to the full path name of the
 *                        proposed DLL. This parameter must include a file
 *                        name.  A directory specification is inadequate.
 *
 *      Returns: TRUE if no files were found or FALSE otherwise.
 */
BOOL IDE_EXPORT WatIDE_DirIsOk( char *dllname );

/* WatIDE_RunWig - This function is used to generate C++ code from an SRU
 *              file.  It should be called once for each C++ user object
 *              to be created.  If one object inherits from another you must
 *              call this function for each object.
 *
 *      Parameters
 *              sruname - this should point to the full path name of the
 *                        .sru file to process. sruname must be non-empty
 *                        and non-NULL.
 *              parentname - if this user object inherits from another this
 *                        parameter should point to the name of the parent
 *                        object.  If this object does not inherit from
 *                        another set parentname to NULL.
 *              info - if this parameter is non-NULL on entry and any errors
 *                      or warnings are generated during processing then
 *                      on exit *info will point to a WatIDEErrInfo structure
 *                      containing information about the errors. If *info
 *                      is non-Null PB should display the error information
 *                      to the user.
 *
 *      Returns: TRUE if a error occurred and processing could not be
 *               completed or FALSE otherwise.
*/
BOOL IDE_EXPORT WatIDE_RunWig( char *sruname, char *parentname,
                               WatIDEErrInfo **info );

/*
 * WatIDE_FiniErrInfo - This function should be called to free memory
 *                      allocated to a WatIDEErrInfo structure returned
 *                      by a call to WatIDE_RunWig.
 *      Parameters
 *              info - pointer to a WatIDEErrInfo structure returned by
 *                      a call to WatIDE_RunWig
 */
void IDE_EXPORT WatIDE_FiniErrInfo( WatIDEErrInfo *info );

/*
 * WatIDE_RunIDE - This function should be called to start the WATCOM IDE
 *              or cause it to load a new project.
 *      Parameters
 *              dllname - should specify the full path of the dll to be
 *                      built.  This path should be the same as the path
 *                      specified when WatIDE_RunWig was called to generate
 *                      the stub code for the DLL. dllname must be non-empty
 *                      and non-NULL.
 *
 *      Returns: FALSE if the IDE was successfully opened the project or
 *               TRUE otherwise.
 *
 *      Notes
 *              PB should ensure that code for separate DLL's is kept in
 *              separate directories.  When a DLL contains a UO that
 *              inherits from another all its ancestors must have their
 *              code generated in the same directory and included in the
 *              same DLL.
 */
BOOL IDE_EXPORT WatIDE_RunIDE( char *dllname );

/*
 * WatIDE_CloseIDE - used to close the IDE.  This should be called when
 *              PB is shutting down.
 *
 *      Returns: FALSE if the IDE shut down or TRUE otherwise.
 */
BOOL IDE_EXPORT WatIDE_CloseIDE( void );

/*
 * WatIDE_GetFunctionName - get the name of the cover function that will
 *                          be generated for a given function
 *      Parameters
 *              type    specifies the type of operation to be preformed.
 *                      FN_GET_CONSTRUCTOR - return the name for the
 *                              constructor for this user object.
 *                      FN_GET_DESTRUCTOR - return the name for the
 *                              destructor for this user object
 *                      FN_GET_FUNCTION - return the name for the cover
 *                              function for the method specified by fnname
 *              buf     points to a buffer of at least 41 characters.
 *                      On exit the buffer will contain the generated cover
 *                      function name.
 *              uoname  contains the name of the user object to which the
 *                      function belongs
 *              fnname  If type == FN_GET_FUNCTION this is the name of the
 *                      function for which to generate a cover function name.
 *                      This parameter should be NULL if
 *                      type != FN_GET_FUNCTION.
 */
void IDE_EXPORT WatIDE_GetFunctionName(  FunctionRequest type, char *buf,
                                         char *uoname, char *fnname );

#endif
