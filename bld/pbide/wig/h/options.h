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


#ifndef OPTIONS_MANAGER_DEFINED
#define OPTIONS_MANAGER_DEFINED

extern char     *GetInputFile(void);
extern char     *GetOutputFile(void);
extern char     *GetCoverFile(void);
extern char     *GetHeaderFile(void);
extern char     *GetCodeFile(void);
extern char     *GetClass(void);
extern char     *GetLibName(void);
extern char     *GetPrefix(void);
extern char     *GetParentClass( void );
extern char     *GetParentHeader( void );
extern char     *GetErrorFile( void );
extern char     *GetLmainName( void );
extern void     GetHash( const char *uoname, char *hash, unsigned len );
extern void     SetBaseName(const char *);
extern BOOL     ProcessOptions(int, char **);
extern void     PostProcessOptions( void );

extern unsigned long            Options;

#define OPT_GEN_C_CODE          0x00000001
#define OPT_GEN_RETURN          0x00000002
#define OPT_INPUT_FILE_SET      0x00000004
#define OPT_OUTPUT_FILE_SET     0x00000008
#define OPT_CODE_FILE_SET       0x00000010
#define OPT_HEADER_FILE_SET     0x00000020
#define OPT_COVER_FILE_SET      0x00000040
#define OPT_CLASS_NAME_SET      0x00000080
#define OPT_LIBRARY_NAME_SET    0x00000100
#define OPT_PREFIX_SET          0x00000200
#define OPT_MODIFY_SRU          0x00000400
#define OPT_ERROR_FILE_SET      0x00000800
#define OPT_PARENT_SET          0x00001000
#define OPT_PARENT_HEADER_SET   0x00002000

#endif
