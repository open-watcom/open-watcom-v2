// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/
/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  ICU single converter minimised loader
*                   Only support for MBCS/SBCS/UTF-8 (ASCII encoding)
*
****************************************************************************/


#include "unicode/utypes.h"
#include "udatamem.h"
#include "cmemory.h"
#include "ucnv_imp.h"


class ICULoader {
public:
    ICULoader( const char *name );
    ~ICULoader();

    UConverter *clone( UErrorCode *err );
    void close( UConverter *converter );
    UChar32 getNextUChar( UConverter *converter, const char **source, const char *source_end, UErrorCode *err );
    void fromUnicodeToMBCS( UConverter *converter, char **target, const char *target_end, const UChar **source, const UChar *source_end, int32_t *offset, UBool flush, UErrorCode *err );
private:
    UConverterSharedData *createFromFile( UConverterLoadArgs *pArgs, UErrorCode *err );
    void unloadSharedData( UConverter *converter );

private:
    UConverter              _converter;
};
