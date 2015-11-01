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
 * returns: FN_RC_FAILED_TO_INITIALIZE
 *          FN_RC_NO_FILE_SELECTED
 *          FN_RC_FILE_SELECTED
 *          FN_RC_RUNTIME_ERROR
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
 *              FN_OVERWRITEPROMPT - prompts the user if he selects a file
 *                                    that already exists
 *              FN_HIDEREADONLY    - read only files will not be displayed.
 *              FN_CHANGEDIR       - once the dialog is exited, the working
 *                                    directory will be the home directory
 *                                    of the selected file.
 *              FN_ISSAVE          - getting a file name for saving
 *
 * initial_dir - used to specify the starting directory.  Using NULL will
 *               cause the current directory to be the starting directory.
 *
 * wnd - the parent window of the dialog.
 *
 */

enum {
    FN_RC_FAILED_TO_INITIALIZE,
    FN_RC_NO_FILE_SELECTED,
    FN_RC_FILE_SELECTED,
    FN_RC_RUNTIME_ERROR
};

typedef enum {
    FN_OVERWRITEPROMPT  = 0x0001,
    FN_HIDEREADONLY     = 0x0002,
    FN_CHANGEDIR        = 0x0004,
    FN_ALLOWMULTISELECT = 0x0008,
    FN_PATHMUSTEXIST    = 0x0010,
    FN_FILEMUSTEXIST    = 0x0020,
    FN_WANT_LAST_PATH   = 0x0040,
    FN_ISSAVE           = 0x0080,
} fn_flags;

typedef struct
{
    const char          *filter_list;
    int                 filter_index;
    char                *file_name;
    int                 max_file_name;
    char                *base_file_name;
    int                 max_base_file_name;
    char                *title;
    fn_flags            flags;
    char                *initial_dir;
    char                *last_path;
} open_file_name;

int GUIGetFileName( gui_window *wnd, open_file_name *ofn );
