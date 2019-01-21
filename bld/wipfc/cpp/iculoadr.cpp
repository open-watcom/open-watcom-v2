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


#include "wipfc.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "env.hpp"
#include "util.hpp"
#include "iculoadr.hpp"


/*
 * Minimised version of ICU loader for one converter only
 * no any data sharing/caching
 */
static const UConverterSharedData * const converterData[] = {
    NULL,
    NULL,
    &_MBCSData,
    NULL,
    &_UTF8Data
};

static void dataReset( UDataMemory *dataMemory )
{
    std::memset( dataMemory, 0, sizeof( *dataMemory ) );
    dataMemory->length = -1;
}

static bool mapFile( UDataMemory *dataMemory, const char *name )
{
    std::FILE       *fp;
    dword           fileSize;
    char            *p;
    bool            ok = false;

    std::string path( Environment.value( "WIPFC" ) );
    if( path.length() )
        path += PATH_SEPARATOR;
    path += name;
    path += ".cnv";
    dataReset( dataMemory );
    fp = std::fopen( path.c_str(), "rb" );
    if( fp != NULL ) {
        std::fseek( fp, 0, SEEK_END );
        fileSize = std::ftell( fp );
        std::fseek( fp, 0, SEEK_SET );
        if( !std::ferror( fp ) && fileSize > 20 ) {
            p = new char[fileSize];
            if( p != NULL ) {
                if( fileSize != std::fread( p, 1, fileSize, fp ) ) {
                    delete[] p;
                } else {
                    dataMemory->map = p;
                    dataMemory->pHeader = (const DataHeader *)p;
                    dataMemory->mapAddr = p;
                    dataMemory->length = (int32_t)fileSize;
                    ok = true;
                }
            }
        }
        std::fclose( fp );
    }
    return ok;
}

static void unmapFile( UDataMemory *dataMemory )
{
    if( dataMemory != NULL && dataMemory->map != NULL ) {
        delete[] (char *)dataMemory->map;
        dataReset( dataMemory );
    }
}

void ICULoader::unloadSharedData( UConverter *converter )
{
    if( converter->sharedData != NULL ) {
        if( converter->sharedData->referenceCounter > 0 ) {
            converter->sharedData->referenceCounter--;
        }
        if( converter->sharedData->referenceCounter <= 0 ) {
            if( converter->sharedData->impl->unload != NULL ) {
                converter->sharedData->impl->unload( converter->sharedData );
            }
            if( converter->sharedData->dataMemory != NULL ) {
                UDataMemory *dataMemory = (UDataMemory *)converter->sharedData->dataMemory;
                unmapFile( dataMemory );
                delete dataMemory;
            }
            delete converter->sharedData;
            converter->sharedData = NULL;
        }
    }
}

static word dataGetHeaderSize(const DataHeader *udh)
{
    if( udh == NULL ) {
        return 0;
    } else if( udh->info.isBigEndian == U_IS_BIG_ENDIAN ) {
        /* same endianness */
        return udh->dataHeader.headerSize;
    } else {
        /* opposite endianness */
        word x = udh->dataHeader.headerSize;
        return (word)(( x << 8 ) | ( x >> 8 ));
    }
}

static UConverterSharedData *unFlattenClone( UConverterLoadArgs *pArgs, UDataMemory *dataMemory, UErrorCode *err )
{
    const uint8_t *raw = (const uint8_t *)(dataMemory->pHeader) + dataGetHeaderSize( dataMemory->pHeader );
    const UConverterStaticData *source = (const UConverterStaticData *)raw;
    UConverterSharedData *data;
    UConverterType type = (UConverterType)source->conversionType;

    if( U_FAILURE( *err ) )
        return NULL;

    if( (word)type > UCNV_MBCS ||
        converterData[type] == NULL ||
        !converterData[type]->isReferenceCounted ||
        converterData[type]->referenceCounter != 1 ||
        source->structSize != sizeof( UConverterStaticData ) )
    {
        *err = U_INVALID_TABLE_FORMAT;
        return NULL;
    }

    data = new UConverterSharedData;
    if( data == NULL ) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    /* copy initial values from the static structure for this type */
    std::memcpy( data, converterData[type], sizeof( UConverterSharedData ) );
    data->staticData = source;
    data->sharedDataCached = FALSE;
    data->dataMemory = (void *)dataMemory;

    if( data->impl->load != NULL ) {
        data->impl->load( data, pArgs, raw + source->structSize, err );
        if( U_FAILURE( *err ) ) {
            delete data;
            return NULL;
        }
    }
    return data;
}

static bool checkHeader( const DataHeader *pHeader )
{
    return( pHeader->dataHeader.magic1 == 0xda
      && pHeader->dataHeader.magic2 == 0x27
      && pHeader->info.size >= 20
      && pHeader->info.isBigEndian   == U_IS_BIG_ENDIAN
      && pHeader->info.charsetFamily == U_CHARSET_FAMILY
      && pHeader->info.sizeofUChar   == U_SIZEOF_UCHAR
      && pHeader->info.dataFormat[0] == 0x63       /* dataFormat="cnvt" */
      && pHeader->info.dataFormat[1] == 0x6e
      && pHeader->info.dataFormat[2] == 0x76
      && pHeader->info.dataFormat[3] == 0x74
      && pHeader->info.formatVersion[0] == 6 );    /* Everything will be version 6 */
}

UConverterSharedData * ICULoader::createFromFile( UConverterLoadArgs *pArgs, UErrorCode *err )
{
    UDataMemory             *data;
    UConverterSharedData    *sharedData;
    UDataMemory             dataMemory;

    if( U_FAILURE( *err ) ) {
        return NULL;
    }

    data = NULL;
    if( mapFile( &dataMemory, pArgs->name ) ) {
        if( checkHeader( dataMemory.pHeader ) ) {
            data = new UDataMemory;
            if( data == NULL ) {
                *err = U_MEMORY_ALLOCATION_ERROR;
            } else {
                *data = dataMemory;
                data->heapAllocated = true;
                sharedData = unFlattenClone( pArgs, data, err );
                if( sharedData != NULL ) {
                    return sharedData;
                }
                delete data;
            }
        } else {
            /* the data is not acceptable, or some error occured.  Either way, unmap the memory */
            *err = U_INVALID_FORMAT_ERROR;
        }
        unmapFile( &dataMemory );
    } else {
        /* file not found or other file access errors */
        *err = U_FILE_ACCESS_ERROR;
    }
    return NULL;
}

void  ICULoader::close( UConverter *converter )
{
    UErrorCode errorCode = U_ZERO_ERROR;

    /* In order to speed up the close, only call the callbacks when they have been changed.
    This performance check will only work when the callbacks are set within a shared library
    or from user code that statically links this code. */
    /* first, notify the callback functions that the converter is closed */
    if( converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK ) {
        UConverterToUnicodeArgs toUArgs = { sizeof( UConverterToUnicodeArgs ), TRUE, NULL, NULL, NULL, NULL, NULL, NULL };
        toUArgs.converter = &_converter;
        errorCode = U_ZERO_ERROR;
        converter->fromCharErrorBehaviour( converter->toUContext, &toUArgs, NULL, 0, UCNV_CLOSE, &errorCode );
    }
    if( converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK ) {
        UConverterFromUnicodeArgs fromUArgs = { sizeof( UConverterFromUnicodeArgs ), TRUE, NULL, NULL, NULL, NULL, NULL, NULL };
        fromUArgs.converter = &_converter;
        errorCode = U_ZERO_ERROR;
        converter->fromUCharErrorBehaviour( converter->fromUContext, &fromUArgs, NULL, 0, 0, UCNV_CLOSE, &errorCode );
    }

    if( converter->sharedData->impl->close != NULL ) {
        converter->sharedData->impl->close( converter );
    }

    if( converter->subChars != (uint8_t *)converter->subUChars ) {
        delete[] converter->subChars;
    }

    if( converter->sharedData->isReferenceCounted ) {
        unloadSharedData( converter );
    }
}

UConverter* ICULoader::clone( UErrorCode *err )
{
    UConverter *localConverter, *allocatedConverter;
    int32_t bufferSizeNeeded;
    UErrorCode cbErr;
    UConverterToUnicodeArgs toUArgs = { sizeof( UConverterToUnicodeArgs ), TRUE, NULL, NULL, NULL, NULL, NULL, NULL };
    UConverterFromUnicodeArgs fromUArgs = { sizeof( UConverterFromUnicodeArgs ), TRUE, NULL, NULL, NULL, NULL, NULL, NULL };

    if( err == NULL || U_FAILURE( *err ) ) {
        return NULL;
    }

    if(_converter.sharedData->impl->safeClone != NULL) {
        /* call the custom safeClone function for sizing */
        bufferSizeNeeded = 0;
        _converter.sharedData->impl->safeClone(&_converter, NULL, &bufferSizeNeeded, err);
        if (U_FAILURE(*err)) {
            return NULL;
        }
    } else {
        /* inherent sizing */
        bufferSizeNeeded = sizeof(UConverter);
    }

    /* allocate one here...*/
    localConverter = allocatedConverter = (UConverter *)new char[bufferSizeNeeded];

    if(localConverter == NULL) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }
    *err = U_SAFECLONE_ALLOCATED_WARNING;

    std::memset( allocatedConverter, 0, bufferSizeNeeded );

    /* Copy initial state */
    std::memcpy( allocatedConverter, &_converter, sizeof( UConverter ) );
    allocatedConverter->isCopyLocal = allocatedConverter->isExtraLocal = FALSE;

    /* copy the substitution string */
    if( _converter.subChars == (uint8_t *)_converter.subUChars ) {
        allocatedConverter->subChars = (uint8_t *)allocatedConverter->subUChars;
    } else {
        allocatedConverter->subChars = new uint8_t[UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR];
        if( allocatedConverter->subChars == NULL ) {
            return NULL;
        }
        std::memcpy( allocatedConverter->subChars, _converter.subChars, UCNV_ERROR_BUFFER_LENGTH * U_SIZEOF_UCHAR );
    }

    /* now either call the safeclone fcn or not */
    if( _converter.sharedData->impl->safeClone != NULL ) {
        /* call the custom safeClone function */
        localConverter = _converter.sharedData->impl->safeClone( &_converter, allocatedConverter, &bufferSizeNeeded, err );
    }

    if( localConverter == NULL || U_FAILURE( *err ) ) {
        if( allocatedConverter != NULL && allocatedConverter->subChars != (uint8_t *)allocatedConverter->subUChars ) {
            delete[] allocatedConverter->subChars;
        }
        delete[] (char *)allocatedConverter;
        return NULL;
    }

    /* increment refcount of shared data if needed */
    if( _converter.sharedData->isReferenceCounted ) {
        _converter.sharedData->referenceCounter++;
    }

    /* allow callback functions to handle any memory allocation */
    toUArgs.converter = fromUArgs.converter = localConverter;
    cbErr = U_ZERO_ERROR;
    _converter.fromCharErrorBehaviour( _converter.toUContext, &toUArgs, NULL, 0, UCNV_CLONE, &cbErr );
    cbErr = U_ZERO_ERROR;
    _converter.fromUCharErrorBehaviour( _converter.fromUContext, &fromUArgs, NULL, 0, 0, UCNV_CLONE, &cbErr );

    return localConverter;
}

UChar32 ICULoader::getNextUChar( UConverter *converter, const char **start, const char *end, UErrorCode *err )
{
    return ucnv_getNextUChar( converter, start, end, err );
}

void ICULoader::fromUnicodeToMBCS( UConverter *converter, char **target, const char *target_end, const UChar **source, const UChar *source_end, int32_t *offset, UBool flush, UErrorCode *err )
{
    ucnv_fromUnicode( converter, target, target_end, source, source_end, offset, flush, err );
}

bool ICULoader::useDBCS()
{
    return _converter.sharedData->staticData->minBytesPerChar == 1 && _converter.sharedData->staticData->maxBytesPerChar == 2;
}

ICULoader::ICULoader( const char *name )
{
    UErrorCode err = U_ZERO_ERROR;
    UConverterLoadArgs  args = UCNV_LOAD_ARGS_INITIALIZER;

    std::memset( &_converter, 0, sizeof( _converter ) );

    args.name = name;

    if( std::strcmp( args.name, "utf-8" ) == 0 ) {
        /* algorithmic converter */
        _converter.sharedData = (UConverterSharedData *)converterData[UCNV_UTF8];
    } else {
        /* data-based converter, get its data from a file */
        args.nestedLoads = 1;
        _converter.sharedData = createFromFile( &args, &err );
        if( U_FAILURE( err ) ) {
            return;
        }
    }

    /* initialize the _converter */
    _converter.isCopyLocal = TRUE;
    _converter.isExtraLocal = FALSE;
    _converter.options = args.options;

    _converter.preFromUFirstCP = U_SENTINEL;
    _converter.fromCharErrorBehaviour = UCNV_TO_U_DEFAULT_CALLBACK;
    _converter.fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
    _converter.toUnicodeStatus = _converter.sharedData->toUnicodeStatus;
    _converter.maxBytesPerUChar = _converter.sharedData->staticData->maxBytesPerChar;
    _converter.subChar1 = _converter.sharedData->staticData->subChar1;
    _converter.subCharLen = _converter.sharedData->staticData->subCharLen;
    _converter.subChars = (uint8_t *)_converter.subUChars;
    std::memcpy( _converter.subChars, _converter.sharedData->staticData->subChar, _converter.subCharLen );
    _converter.toUCallbackReason = UCNV_ILLEGAL; /* default reason to invoke ( *fromCharErrorBehaviour ) */

    if( _converter.sharedData->impl->open != NULL ) {
        _converter.sharedData->impl->open( &_converter, &args, &err );
        if( U_FAILURE( err ) ) {
            close( &_converter );
        }
    }
}

ICULoader::~ICULoader()
{
    close( &_converter );
}
