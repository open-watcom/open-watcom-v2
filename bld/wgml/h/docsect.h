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
* Description:  Declaration of various document sections constants
*
****************************************************************************/


pick( "NONE",      doc_sect_none,     no_ban       )
pick( "GDOC",      doc_sect_gdoc,     no_ban       )
pick( "FRONTM",    doc_sect_frontm,   no_ban       )
pick( "TITLEP",    doc_sect_titlep,   no_ban       )
pick( "eTITLEP",   doc_sect_etitlep,  no_ban       )
pick( "ABSTRACT",  doc_sect_abstract, abstract_ban )
pick( "PREFACE",   doc_sect_preface,  preface_ban  )
pick( "TOC",       doc_sect_toc,      toc_ban      )
pick( "FIGLIST",   doc_sect_figlist,  figlist_ban  )
pick( "BODY",      doc_sect_body,     body_ban     )
pick( "APPENDIX",  doc_sect_appendix, appendix_ban )
pick( "BACKM",     doc_sect_backm,    backm_ban    )
pick( "INDEX",     doc_sect_index,    index_ban    )
pick( "eGDOC",     doc_sect_egdoc,    no_ban       )
