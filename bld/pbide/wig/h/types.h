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


#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

enum {
        /* standard types */
        TY_NONE,
        TY_BLOB,
        TY_BOOLEAN,
        TY_CHAR,
        TY_DATETIME,
        TY_DATE,
        TY_DEC,
        TY_DOUBLE,
        TY_INT,
        TY_LONG,
        TY_REAL,
        TY_STRING,
        TY_TIME,
        TY_UINT,
        TY_ULONG,

        /* enumerate types */
        TY_ALIGNMENT,
        TY_ARRANGETYPES,
        TY_BORDER,
        TY_BORDERSTYLE,
        TY_BUTTON,
        TY_CONVERTTYPE,
        TY_DRAGMODES,
        TY_DWBUFFER,
        TY_DWITEMSTATUS,
        TY_FILEACCESS,
        TY_FILELOCK,
        TY_FILEMODE,
        TY_FILLPATTERN,
        TY_FFONTCHARSET,
        TY_FONTFAMILY,
        TY_FONTPITCH,
        TY_GRCOLORTYPE,
        TY_GRDATATYPE,
        TY_GRGRAPHTYPE,
        TY_GRLEGENDTYPE,
        TY_GROBJECTTYPE,
        TY_GRRESETTYPE,
        TY_GRSCALETYPE,
        TY_GRSCALEVALUE,
        TY_GRSYMBOLTYPE,
        TY_GRTICTYPE,
        TY_HELPCOMMAND,
        TY_ICON,
        TY_KEYCODE,
        TY_LIBDIRTYPE,
        TY_LIBEXPORTTYPE,
        TY_LIBIMPORTTYPE,
        TY_LINESTYLE,
        TY_MAILFILETYPE,
        TY_MAILLOGONOPTION,
        TY_MAILREADOPTION,
        TY_MAILRECIPIENTTYPE,
        TY_MAILRETURNCODE,
        TY_MASKDATATYPE,
        TY_OBJECT,
        TY_PARMTYPE,
        TY_POINTER,
        TY_ROWFOCUSIND,
        TY_SAVEASTYPE,
        TY_SEEKTYPE,
        TY_SEEKPOSTYPE,
        TY_TEXTCASE,
        TY_TOOLBARALIGNMENT,
        TY_TRIGEVENT,
        TY_USEROBJECTS,
        TY_VTEXTALIGN,
        TY_WINDOWSTATE,
        TY_WINDOWTYPE,
        TY_WRITEMODE,

        /* system types */
        TY_APPLICATION,
        TY_CHECKBOX,
        TY_COMMANDBUTTON,
        TY_DATAWINDOW,
        TY_DATAWINDOWCHILD,
        TY_DRAGOBJECT,
        TY_DRAWOBJECT,
        TY_DROPDOWNLISTBOX,
        TY_DYNAMICDESCRIPTIONAREA,
        TY_DYNAMICSTAGINGAREA,
        TY_EDITMASK,
        TY_ERROR,
        TY_FUNCTION_OBJECT,
        TY_GRAPH,
        TY_GRAPHOBJECT,
        TY_GRAXIS,
        TY_GRDISPATTR,
        TY_GROUPBOX,
        TY_HSCROLLBAR,
        TY_LINE,
        TY_LISTBOX,
        TY_MAILFILEDESCRIPTION,
        TY_MAILMESSAGE,
        TY_MAILRECIPIENT,
        TY_MAILSESSION,
        TY_MDICLIENT,
        TY_MENU,
        TY_MESSAGE,
        TY_MULTILINEEDIT,
        TY_NONVISUALOBJECT,
        TY_OVAL,
        TY_PICTURE,
        TY_PICTUREBUTTON,
        TY_POWEROBJECT,
        TY_RADIOBUTTON,
        TY_RECTANGLE,
        TY_ROUNDRECTANGLE,
        TY_SINGLELINEEDIT,
        TY_STATICTEXT,
        TY_STRUCTURE,
        TY_SYSTEMFUNCTIONS,
        TY_TRANSACTION,
        TY_USEROBJECT,
        TY_VSCROLLBAR,
        TY_WINDOW,
        TY_WINDOWOBJECT,

        TY_COUNT_OF_TYPES
};

#define TY_TYPE_MASK            0x000000FF
#define TY_REF_INDICATOR        0x00000100
#define TY_LAST_TYPE            TY_COUNT_OF_TYPES - 1

typedef long    id_type;

#define _IsIdType( id ) ((id > TY_NONE) && (id<=TY_LAST_TYPE))
#define _IsRefType( id ) ( (id & TY_REF_INDICATOR) != 0 )
#define _BaseType( id ) (id & TY_TYPE_MASK)

#endif
