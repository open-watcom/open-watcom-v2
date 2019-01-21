// (c) 2016 and later: Unicode, Inc. and others.
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
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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
    bool useDBCS();
private:
    UConverterSharedData *createFromFile( UConverterLoadArgs *pArgs, UErrorCode *err );
    void unloadSharedData( UConverter *converter );

private:
    UConverter              _converter;
};
