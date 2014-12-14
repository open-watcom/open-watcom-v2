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


/*
 * GUIGetFileName
 * --------------
 * returns: OFN_RC_FAILED_TO_INITIALIZE
 *          OFN_RC_NO_FILE_SELECTED
 *          OFN_RC_FILE_SELECTED
 *          OFN_RC_RUNTIME_ERROR
 *
 * open_file_name structure
 * ------------------------
 *
 * filter_list - a set of null delimited string pairs describing the files
 *               that can be edited.  The first string in the pair is
 *               a description, the second string is a wild card.  The
 *               second string may contain semicolons to separate multiple
 *               wild cards.  The list must be terminated by two null chars.
 *               For example:
 *                      char *filterList =
 *                              "Text Files\0*.txt\0" \
 *                              "All Files\0*.*\0" \
 *                              "Source files\0*.c;*.h;*.cpp;*.hpp;*.asm\0" \
 *                              "\0";
 *
 * filter_index - a zero-based index, indicating which of the items in
 *                the filter list will be displayed when the dialog is
 *                created.
 *
 * file_name - a pointer to a buffer for storing the fully qualified path
 *             of the file that the user selects.  May be NULL if this
 *             is not desired.
 *
 * max_file_name - the length of the file_name buffer.
 *
 * base_file_name - a pointer to a buffer for storing the file name and
 *                  extension of the file that the user selects.  May be NULL
 *                  if this is not desired.
 *
 * max_base_file_name - the length of the base_file_name buffer.
 *
 * title - the title to be assigned to to dialog
 *
 * flags - flags to control the behavior of the dialog.  They are:
 *              OFN_OVERWRITEPROMPT - prompts the user if he selects a file
 *                                    that already exists
 *              OFN_HIDEREADONLY    - read only files will not be displayed.
 *              OFN_CHANGEDIR       - once the dialog is exited, the working
 *                                    directory will be the home directory
 *                                    of the selected file.
 *              OFN_ISSAVE          - getting a file name for saving
 *
 * initial_dir - used to specify the starting directory.  Using NULL will
 *               cause the current directory to be the starting directory.
 *
 * wnd - the parent window of the dialog.
 *
 */

/* these are often part of comdlg.h - no need to
   define twice */

#ifndef OFN_OVERWRITEPROMPT
#define OFN_OVERWRITEPROMPT             0x00000002
#endif

#ifndef OFN_HIDEREADONLY
#define OFN_HIDEREADONLY                0x00000004
#endif

#ifndef OFN_CHANGEDIR
#define OFN_CHANGEDIR                   0x00000008
#endif

#ifndef OFN_ALLOWMULTISELECT
#define OFN_ALLOWMULTISELECT            0x00000200
#endif

#ifndef OFN_PATHMUSTEXIST
#define OFN_PATHMUSTEXIST               0x00000800
#endif

#ifndef OFN_FILEMUSTEXIST
#define OFN_FILEMUSTEXIST               0x00001000
#endif

#ifndef OFN_WANT_LAST_PATH
#define OFN_WANT_LAST_PATH              0x40000000
#endif

#ifndef OFN_ISSAVE
#define OFN_ISSAVE                      0x80000000
#endif

enum {
    OFN_RC_FAILED_TO_INITIALIZE,
    OFN_RC_NO_FILE_SELECTED,
    OFN_RC_FILE_SELECTED,
    OFN_RC_RUNTIME_ERROR
};

typedef struct
{
    const char          *filter_list;
    int                 filter_index;
    char                *file_name;
    int                 max_file_name;
    char                *base_file_name;
    int                 max_base_file_name;
    char                *title;
    unsigned long       flags;
    char                *initial_dir;
    char                *last_path;
} open_file_name;

int GUIGetFileName( gui_window *wnd, open_file_name *ofn );
