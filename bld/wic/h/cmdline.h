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


typedef enum {
    TLT_FORTRAN,
    TLT_ASM,
    TLT_MAX,
    TLT_NONE  = TLT_MAX,
    TLT_C
} TargetLangType;

typedef enum {
    SIZE_16,              // 2 bytes
    SIZE_32,              // 4 bytes
    SIZE_48,              // 6 bytes
    SIZE_MAX
} SizeType;

struct Opt {
    TargetLangType targetLang: 2;
    int signedChar: 1;
    int asmAttachD: 1;
    int prefixStructFields;
    char *structFieldsSep;
    char *conflictPrefix;
    int supressLevel;
    unsigned outLineLen;
    pSLList fileNameList;
    pSLList incPathList;
    SizeType intSize;
    SizeType nearPtrSize;
    SizeType ptrSize;
    SizeType farPtrSize;
    SizeType hugePtrSize;
    int debug;  // Debugging flag, can be turned on by using '-d' option
};

void getCmdLineOptions(int argc, char *argv[]);
void zapCmdLineOptions(void);

/*------------------------------------------------------------------*/

typedef struct {
    int readOnly;  // File/directory is read-only
    char *name;
} FDReadInd, *pFDReadInd;  // File/directory read indicator
void initFDReadInd(pFDReadInd entry, char *name, int readOnly);
pFDReadInd createFDReadInd(char *name, int readOnly);
void zapFDReadInd(pFDReadInd entry);
    // NOTE: Only name differentiates the two elements for hash tables
unsigned FDReadIndHashFunc(pFDReadInd entry, unsigned size);
int FDReadIndCmpFunc(pFDReadInd entry1, pFDReadInd entry2);
