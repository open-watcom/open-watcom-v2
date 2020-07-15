/*****************************************************************************
*
*                          Open Watcom Project
*
* Copyright (c) 2018-2020 The Open Watcom Contributors. All Rights Reserved.
*
*****************************************************************************/
// (c) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  ucnv.c:
*  Implements APIs for the ICU's codeset conversion library;
*  mostly calls through internal functions;
*  created by Bertrand A. Damiba
*
* Modification History:
*
*   Date        Name        Description
*   04/04/99    helena      Fixed internal header inclusion.
*   05/09/00    helena      Added implementation to handle fallback mappings.
*   06/20/2000  helena      OS/400 port changes; mostly typecast.
*/

#include "unicode/utypes.h"

#if !UCONFIG_NO_CONVERSION

#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#include "unicode/ucnv_err.h"
#include "unicode/uset.h"
#include "unicode/utf.h"
#include "unicode/utf16.h"
#include "putilimp.h"
#include "cmemory.h"
#include "cstring.h"
#include "uassert.h"
#include "utracimp.h"
#include "ustr_imp.h"
#include "ucnv_imp.h"
#include "ucnv_cnv.h"
#include "ucnv_bld.h"
#include "cwchar.h"


U_CAPI int32_t   U_EXPORT2
u_strlen(const UChar *s)
{
#if U_SIZEOF_WCHAR_T == U_SIZEOF_UCHAR
    return (int32_t)uprv_wcslen((const wchar_t *)s);
#else
    const UChar *t = s;
    while(*t != 0) {
      ++t;
    }
    return t - s;
#endif
}

/* NUL-termination of strings ----------------------------------------------- */

/**
 * NUL-terminate a string no matter what its type.
 * Set warning and error codes accordingly.
 */
#define __TERMINATE_STRING(dest, destCapacity, length, pErrorCode)      \
    if(pErrorCode!=NULL && U_SUCCESS(*pErrorCode)) {                    \
        /* not a public function, so no complete argument checking */   \
                                                                        \
        if(length<0) {                                                  \
            /* assume that the caller handles this */                   \
        } else if(length<destCapacity) {                                \
            /* NUL-terminate the string, the NUL fits */                \
            dest[length]=0;                                             \
            /* unset the not-terminated warning but leave all others */ \
            if(*pErrorCode==U_STRING_NOT_TERMINATED_WARNING) {          \
                *pErrorCode=U_ZERO_ERROR;                               \
            }                                                           \
        } else if(length==destCapacity) {                               \
            /* unable to NUL-terminate, but the string itself fit - set a warning code */ \
            *pErrorCode=U_STRING_NOT_TERMINATED_WARNING;                \
        } else /* length>destCapacity */ {                              \
            /* even the string itself did not fit - set an error code */ \
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;                        \
        }                                                               \
    }

U_CAPI int32_t U_EXPORT2
u_terminateUChars(UChar *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    return length;
}

U_CAPI int32_t U_EXPORT2
u_terminateChars(char *dest, int32_t destCapacity, int32_t length, UErrorCode *pErrorCode) {
    __TERMINATE_STRING(dest, destCapacity, length, pErrorCode);
    return length;
}

/*resets the internal states of a converter
 *goal : have the same behaviour than a freshly created converter
 */
static void _reset(UConverter *converter, UConverterResetChoice choice,
                   UBool callCallback) {
    if(converter == NULL) {
        return;
    }

    if(callCallback) {
        /* first, notify the callback functions that the converter is reset */
        UErrorCode errorCode;

        if(choice<=UCNV_RESET_TO_UNICODE && converter->fromCharErrorBehaviour != UCNV_TO_U_DEFAULT_CALLBACK) {
            UConverterToUnicodeArgs toUArgs = {
                sizeof(UConverterToUnicodeArgs),
                TRUE,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            };
            toUArgs.converter = converter;
            errorCode = U_ZERO_ERROR;
            converter->fromCharErrorBehaviour(converter->toUContext, &toUArgs, NULL, 0, UCNV_RESET, &errorCode);
        }
        if(choice!=UCNV_RESET_TO_UNICODE && converter->fromUCharErrorBehaviour != UCNV_FROM_U_DEFAULT_CALLBACK) {
            UConverterFromUnicodeArgs fromUArgs = {
                sizeof(UConverterFromUnicodeArgs),
                TRUE,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            };
            fromUArgs.converter = converter;
            errorCode = U_ZERO_ERROR;
            converter->fromUCharErrorBehaviour(converter->fromUContext, &fromUArgs, NULL, 0, 0, UCNV_RESET, &errorCode);
        }
    }

    /* now reset the converter itself */
    if(choice<=UCNV_RESET_TO_UNICODE) {
        converter->toUnicodeStatus = converter->sharedData->toUnicodeStatus;
        converter->mode = 0;
        converter->toULength = 0;
        converter->invalidCharLength = converter->UCharErrorBufferLength = 0;
        converter->preToULength = 0;
    }
    if(choice!=UCNV_RESET_TO_UNICODE) {
        converter->fromUnicodeStatus = 0;
        converter->fromUChar32 = 0;
        converter->invalidUCharLength = converter->charErrorBufferLength = 0;
        converter->preFromUFirstCP = U_SENTINEL;
        converter->preFromULength = 0;
    }

    if (converter->sharedData->impl->reset != NULL) {
        /* call the custom reset function */
        converter->sharedData->impl->reset(converter, choice);
    }
}

static void
_updateOffsets(int32_t *offsets, int32_t length,
               int32_t sourceIndex, int32_t errorInputLength) {
    int32_t *limit;
    int32_t delta, offset;

    if(sourceIndex>=0) {
        /*
         * adjust each offset by adding the previous sourceIndex
         * minus the length of the input sequence that caused an
         * error, if any
         */
        delta=sourceIndex-errorInputLength;
    } else {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         */
        delta=-1;
    }

    limit=offsets+length;
    if(delta==0) {
        /* most common case, nothing to do */
    } else if(delta>0) {
        /* add the delta to each offset (but not if the offset is <0) */
        while(offsets<limit) {
            offset=*offsets;
            if(offset>=0) {
                *offsets=offset+delta;
            }
            ++offsets;
        }
    } else /* delta<0 */ {
        /*
         * set each offset to -1 because this conversion function
         * does not handle offsets
         * or the error input sequence started in a previous buffer
         */
        while(offsets<limit) {
            *offsets++=-1;
        }
    }
}

U_CAPI void  U_EXPORT2
ucnv_resetToUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_TO_UNICODE, TRUE);
}

U_CAPI void  U_EXPORT2
ucnv_resetFromUnicode(UConverter *converter)
{
    _reset(converter, UCNV_RESET_FROM_UNICODE, TRUE);
}


/* ucnv_fromUnicode --------------------------------------------------------- */

/*
 * Implementation note for m:n conversions
 *
 * While collecting source units to find the longest match for m:n conversion,
 * some source units may need to be stored for a partial match.
 * When a second buffer does not yield a match on all of the previously stored
 * source units, then they must be "replayed", i.e., fed back into the converter.
 *
 * The code relies on the fact that replaying will not nest -
 * converting a replay buffer will not result in a replay.
 * This is because a replay is necessary only after the _continuation_ of a
 * partial match failed, but a replay buffer is converted as a whole.
 * It may result in some of its units being stored again for a partial match,
 * but there will not be a continuation _during_ the replay which could fail.
 *
 * It is conceivable that a callback function could call the converter
 * recursively in a way that causes another replay to be stored, but that
 * would be an error in the callback function.
 * Such violations will cause assertion failures in a debug build,
 * and wrong output, but they will not cause a crash.
 */

static void
_fromUnicodeWithCallback(UConverterFromUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterFromUnicode fromUnicode;
    UConverter *cnv;
    const UChar *s;
    char *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t errorInputLength;
    UBool converterSawEndOfInput, calledCallback;

    /* variables for m:n conversion */
    UChar replay[UCNV_EXT_MAX_UCHARS];
    const UChar *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    if(offsets==NULL) {
        fromUnicode=cnv->sharedData->impl->fromUnicode;
    } else {
        fromUnicode=cnv->sharedData->impl->fromUnicodeWithOffsets;
        if(fromUnicode==NULL) {
            /* there is no WithOffsets implementation */
            fromUnicode=cnv->sharedData->impl->fromUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    if(cnv->preFromULength>=0) {
        /* normal mode */
        realSource=NULL;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=NULL;
        realFlush=FALSE;
        realSourceIndex=0;
    } else {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preFromULength;
        pArgs->flush=FALSE;
        sourceIndex=-1;

        cnv->preFromULength=0;
    }

    /*
     * loop for conversion and error handling
     *
     * loop {
     *   convert
     *   loop {
     *     update offsets
     *     handle end of input
     *     handle errors/call callback
     *   }
     * }
     */
    for(;;) {
        if(U_SUCCESS(*err)) {
            /* convert */
            fromUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preFromULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converterSawEndOfInput=
                (UBool)(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->fromUChar32==0);
        } else {
            /* handle error from ucnv_convertEx() */
            converterSawEndOfInput=FALSE;
        }

        /* no callback called yet for this iteration */
        calledCallback=FALSE;

        /* no sourceIndex adjustment for conversion, only for callback output */
        errorInputLength=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        for(;;) {
            /* update offsets if we write any */
            if(offsets!=NULL) {
                int32_t length=(int32_t)(pArgs->target-t);
                if(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, errorInputLength);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                if(sourceIndex>=0) {
                    sourceIndex+=(int32_t)(pArgs->source-s);
                }
            }

            if(cnv->preFromULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                if(realSource==NULL) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preFromU, -cnv->preFromULength*U_SIZEOF_UCHAR);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preFromULength;
                    pArgs->flush=FALSE;
                    if((sourceIndex+=cnv->preFromULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preFromULength=0;
                } else {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==NULL);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            if(U_SUCCESS(*err)) {
                if(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    break;
                } else if(realSource!=NULL) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=NULL;
                    break;
                } else if(pArgs->flush && cnv->fromUChar32!=0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=FALSE; /* new error condition */
                } else {
                    /* input consumed */
                    if(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        if(!converterSawEndOfInput) {
                            break;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_FROM_UNICODE, FALSE);
                    }

                    /* done successfully */
                    return;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                if( calledCallback ||
                    (e=*err)==U_BUFFER_OVERFLOW_ERROR ||
                    (e!=U_INVALID_CHAR_FOUND &&
                     e!=U_ILLEGAL_CHAR_FOUND &&
                     e!=U_TRUNCATED_CHAR_FOUND)
                ) {
                    /*
                     * the callback did not or cannot resolve the error:
                     * set output pointers and return
                     *
                     * the check for buffer overflow is redundant but it is
                     * a high-runner case and hopefully documents the intent
                     * well
                     *
                     * if we were replaying, then the replay buffer must be
                     * copied back into the UConverter
                     * and the real arguments must be restored
                     */
                    if(realSource!=NULL) {
                        int32_t length;

                        U_ASSERT(cnv->preFromULength==0);

                        length=(int32_t)(pArgs->sourceLimit-pArgs->source);
                        if(length>0) {
                            uprv_memcpy(cnv->preFromU, pArgs->source, length);
                            cnv->preFromULength=(int8_t)-length;
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    return;
                }
            }

            /* callback handling */
            {
                UChar32 codePoint;

                /* get and write the code point */
                codePoint=cnv->fromUChar32;
                errorInputLength=0;
                U16_APPEND_UNSAFE(cnv->invalidUCharBuffer, errorInputLength, codePoint);
                cnv->invalidUCharLength=(int8_t)errorInputLength;

                /* set the converter state to deal with the next character */
                cnv->fromUChar32=0;

                /* call the callback function */
                cnv->fromUCharErrorBehaviour(cnv->fromUContext, pArgs,
                    cnv->invalidUCharBuffer, errorInputLength, codePoint,
                    *err==U_INVALID_CHAR_FOUND ? UCNV_UNASSIGNED : UCNV_ILLEGAL,
                    err);
            }

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=TRUE;
        }
    }
}

/*
 * Output the fromUnicode overflow buffer.
 * Call this function if(cnv->charErrorBufferLength>0).
 * @return TRUE if overflow
 */
static UBool
ucnv_outputOverflowFromUnicode(UConverter *cnv,
                               char **target, const char *targetLimit,
                               int32_t **pOffsets,
                               UErrorCode *err) {
    int32_t *offsets;
    char *overflow, *t;
    int32_t i, length;

    t=*target;
    if(pOffsets!=NULL) {
        offsets=*pOffsets;
    } else {
        offsets=NULL;
    }

    overflow=(char *)cnv->charErrorBuffer;
    length=cnv->charErrorBufferLength;
    i=0;
    while(i<length) {
        if(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            do {
                overflow[j++]=overflow[i++];
            } while(i<length);

            cnv->charErrorBufferLength=(int8_t)j;
            *target=t;
            if(offsets!=NULL) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            return TRUE;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        if(offsets!=NULL) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->charErrorBufferLength=0;
    *target=t;
    if(offsets!=NULL) {
        *pOffsets=offsets;
    }
    return FALSE;
}

U_CAPI void U_EXPORT2
ucnv_fromUnicode(UConverter *cnv,
                 char **target, const char *targetLimit,
                 const UChar **source, const UChar *sourceLimit,
                 int32_t *offsets,
                 UBool flush,
                 UErrorCode *err) {
    UConverterFromUnicodeArgs args;
    const UChar *s;
    char *t;

    /* check parameters */
    if(err==NULL || U_FAILURE(*err)) {
        return;
    }

    if(cnv==NULL || target==NULL || source==NULL) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    s=*source;
    t=*target;

    if ((const void *)U_MAX_PTR(sourceLimit) == (const void *)sourceLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a UChar * boundary.
        This also prevents the next argument check from failing.
        */
        sourceLimit = (const UChar *)(((const char *)sourceLimit) - 1);
    }

    /*
     * All these conditions should never happen.
     *
     * 1) Make sure that the limits are >= to the address source or target
     *
     * 2) Make sure that the buffer sizes do not exceed the number range for
     * int32_t because some functions use the size (in units or bytes)
     * rather than comparing pointers, and because offsets are int32_t values.
     *
     * size_t is guaranteed to be unsigned and large enough for the job.
     *
     * Return with an error instead of adjusting the limits because we would
     * not be able to maintain the semantics that either the source must be
     * consumed or the target filled (unless an error occurs).
     * An adjustment would be targetLimit=t+0x7fffffff; for example.
     *
     * 3) Make sure that the user didn't incorrectly cast a UChar * pointer
     * to a char * pointer and provide an incomplete UChar code unit.
     */
    if (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x3fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x7fffffff && targetLimit>t) ||
        (((const char *)sourceLimit-(const char *)s) & 1) != 0)
    {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    /* output the target overflow buffer */
    if( cnv->charErrorBufferLength>0 &&
        ucnv_outputOverflowFromUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        return;
    }
    /* *target may have moved, therefore stop using t */

    if(!flush && s==sourceLimit && cnv->preFromULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        return;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    args.converter=cnv;
    args.flush=flush;
    args.offsets=offsets;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=*target;
    args.targetLimit=targetLimit;
    args.size=sizeof(args);

    _fromUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}


/* ucnv_toUnicode() --------------------------------------------------------- */

static void
_toUnicodeWithCallback(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    UConverterToUnicode toUnicode;
    UConverter *cnv;
    const char *s;
    UChar *t;
    int32_t *offsets;
    int32_t sourceIndex;
    int32_t errorInputLength;
    UBool converterSawEndOfInput, calledCallback;

    /* variables for m:n conversion */
    char replay[UCNV_EXT_MAX_BYTES];
    const char *realSource, *realSourceLimit;
    int32_t realSourceIndex;
    UBool realFlush;

    cnv=pArgs->converter;
    s=pArgs->source;
    t=pArgs->target;
    offsets=pArgs->offsets;

    /* get the converter implementation function */
    sourceIndex=0;
    if(offsets==NULL) {
        toUnicode=cnv->sharedData->impl->toUnicode;
    } else {
        toUnicode=cnv->sharedData->impl->toUnicodeWithOffsets;
        if(toUnicode==NULL) {
            /* there is no WithOffsets implementation */
            toUnicode=cnv->sharedData->impl->toUnicode;
            /* we will write -1 for each offset */
            sourceIndex=-1;
        }
    }

    if(cnv->preToULength>=0) {
        /* normal mode */
        realSource=NULL;

        /* avoid compiler warnings - not otherwise necessary, and the values do not matter */
        realSourceLimit=NULL;
        realFlush=FALSE;
        realSourceIndex=0;
    } else {
        /*
         * Previous m:n conversion stored source units from a partial match
         * and failed to consume all of them.
         * We need to "replay" them from a temporary buffer and convert them first.
         */
        realSource=pArgs->source;
        realSourceLimit=pArgs->sourceLimit;
        realFlush=pArgs->flush;
        realSourceIndex=sourceIndex;

        uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
        pArgs->source=replay;
        pArgs->sourceLimit=replay-cnv->preToULength;
        pArgs->flush=FALSE;
        sourceIndex=-1;

        cnv->preToULength=0;
    }

    /*
     * loop for conversion and error handling
     *
     * loop {
     *   convert
     *   loop {
     *     update offsets
     *     handle end of input
     *     handle errors/call callback
     *   }
     * }
     */
    for(;;) {
        if(U_SUCCESS(*err)) {
            /* convert */
            toUnicode(pArgs, err);

            /*
             * set a flag for whether the converter
             * successfully processed the end of the input
             *
             * need not check cnv->preToULength==0 because a replay (<0) will cause
             * s<sourceLimit before converterSawEndOfInput is checked
             */
            converterSawEndOfInput=
                (UBool)(U_SUCCESS(*err) &&
                        pArgs->flush && pArgs->source==pArgs->sourceLimit &&
                        cnv->toULength==0);
        } else {
            /* handle error from getNextUChar() or ucnv_convertEx() */
            converterSawEndOfInput=FALSE;
        }

        /* no callback called yet for this iteration */
        calledCallback=FALSE;

        /* no sourceIndex adjustment for conversion, only for callback output */
        errorInputLength=0;

        /*
         * loop for offsets and error handling
         *
         * iterates at most 3 times:
         * 1. to clean up after the conversion function
         * 2. after the callback
         * 3. after the callback again if there was truncated input
         */
        for(;;) {
            /* update offsets if we write any */
            if(offsets!=NULL) {
                int32_t length=(int32_t)(pArgs->target-t);
                if(length>0) {
                    _updateOffsets(offsets, length, sourceIndex, errorInputLength);

                    /*
                     * if a converter handles offsets and updates the offsets
                     * pointer at the end, then pArgs->offset should not change
                     * here;
                     * however, some converters do not handle offsets at all
                     * (sourceIndex<0) or may not update the offsets pointer
                     */
                    pArgs->offsets=offsets+=length;
                }

                if(sourceIndex>=0) {
                    sourceIndex+=(int32_t)(pArgs->source-s);
                }
            }

            if(cnv->preToULength<0) {
                /*
                 * switch the source to new replay units (cannot occur while replaying)
                 * after offset handling and before end-of-input and callback handling
                 */
                if(realSource==NULL) {
                    realSource=pArgs->source;
                    realSourceLimit=pArgs->sourceLimit;
                    realFlush=pArgs->flush;
                    realSourceIndex=sourceIndex;

                    uprv_memcpy(replay, cnv->preToU, -cnv->preToULength);
                    pArgs->source=replay;
                    pArgs->sourceLimit=replay-cnv->preToULength;
                    pArgs->flush=FALSE;
                    if((sourceIndex+=cnv->preToULength)<0) {
                        sourceIndex=-1;
                    }

                    cnv->preToULength=0;
                } else {
                    /* see implementation note before _fromUnicodeWithCallback() */
                    U_ASSERT(realSource==NULL);
                    *err=U_INTERNAL_PROGRAM_ERROR;
                }
            }

            /* update pointers */
            s=pArgs->source;
            t=pArgs->target;

            if(U_SUCCESS(*err)) {
                if(s<pArgs->sourceLimit) {
                    /*
                     * continue with the conversion loop while there is still input left
                     * (continue converting by breaking out of only the inner loop)
                     */
                    break;
                } else if(realSource!=NULL) {
                    /* switch back from replaying to the real source and continue */
                    pArgs->source=realSource;
                    pArgs->sourceLimit=realSourceLimit;
                    pArgs->flush=realFlush;
                    sourceIndex=realSourceIndex;

                    realSource=NULL;
                    break;
                } else if(pArgs->flush && cnv->toULength>0) {
                    /*
                     * the entire input stream is consumed
                     * and there is a partial, truncated input sequence left
                     */

                    /* inject an error and continue with callback handling */
                    *err=U_TRUNCATED_CHAR_FOUND;
                    calledCallback=FALSE; /* new error condition */
                } else {
                    /* input consumed */
                    if(pArgs->flush) {
                        /*
                         * return to the conversion loop once more if the flush
                         * flag is set and the conversion function has not
                         * successfully processed the end of the input yet
                         *
                         * (continue converting by breaking out of only the inner loop)
                         */
                        if(!converterSawEndOfInput) {
                            break;
                        }

                        /* reset the converter without calling the callback function */
                        _reset(cnv, UCNV_RESET_TO_UNICODE, FALSE);
                    }

                    /* done successfully */
                    return;
                }
            }

            /* U_FAILURE(*err) */
            {
                UErrorCode e;

                if( calledCallback ||
                    (e=*err)==U_BUFFER_OVERFLOW_ERROR ||
                    (e!=U_INVALID_CHAR_FOUND &&
                     e!=U_ILLEGAL_CHAR_FOUND &&
                     e!=U_TRUNCATED_CHAR_FOUND &&
                     e!=U_ILLEGAL_ESCAPE_SEQUENCE &&
                     e!=U_UNSUPPORTED_ESCAPE_SEQUENCE)
                ) {
                    /*
                     * the callback did not or cannot resolve the error:
                     * set output pointers and return
                     *
                     * the check for buffer overflow is redundant but it is
                     * a high-runner case and hopefully documents the intent
                     * well
                     *
                     * if we were replaying, then the replay buffer must be
                     * copied back into the UConverter
                     * and the real arguments must be restored
                     */
                    if(realSource!=NULL) {
                        int32_t length;

                        U_ASSERT(cnv->preToULength==0);

                        length=(int32_t)(pArgs->sourceLimit-pArgs->source);
                        if(length>0) {
                            uprv_memcpy(cnv->preToU, pArgs->source, length);
                            cnv->preToULength=(int8_t)-length;
                        }

                        pArgs->source=realSource;
                        pArgs->sourceLimit=realSourceLimit;
                        pArgs->flush=realFlush;
                    }

                    return;
                }
            }

            /* copy toUBytes[] to invalidCharBuffer[] */
            errorInputLength=cnv->invalidCharLength=cnv->toULength;
            if(errorInputLength>0) {
                uprv_memcpy(cnv->invalidCharBuffer, cnv->toUBytes, errorInputLength);
            }

            /* set the converter state to deal with the next character */
            cnv->toULength=0;

            /* call the callback function */
            if(cnv->toUCallbackReason==UCNV_ILLEGAL && *err==U_INVALID_CHAR_FOUND) {
                cnv->toUCallbackReason = UCNV_UNASSIGNED;
            }
            cnv->fromCharErrorBehaviour(cnv->toUContext, pArgs,
                cnv->invalidCharBuffer, errorInputLength,
                cnv->toUCallbackReason,
                err);
            cnv->toUCallbackReason = UCNV_ILLEGAL; /* reset to default value */

            /*
             * loop back to the offset handling
             *
             * this flag will indicate after offset handling
             * that a callback was called;
             * if the callback did not resolve the error, then we return
             */
            calledCallback=TRUE;
        }
    }
}

/*
 * Output the toUnicode overflow buffer.
 * Call this function if(cnv->UCharErrorBufferLength>0).
 * @return TRUE if overflow
 */
static UBool
ucnv_outputOverflowToUnicode(UConverter *cnv,
                             UChar **target, const UChar *targetLimit,
                             int32_t **pOffsets,
                             UErrorCode *err) {
    int32_t *offsets;
    UChar *overflow, *t;
    int32_t i, length;

    t=*target;
    if(pOffsets!=NULL) {
        offsets=*pOffsets;
    } else {
        offsets=NULL;
    }

    overflow=cnv->UCharErrorBuffer;
    length=cnv->UCharErrorBufferLength;
    i=0;
    while(i<length) {
        if(t==targetLimit) {
            /* the overflow buffer contains too much, keep the rest */
            int32_t j=0;

            do {
                overflow[j++]=overflow[i++];
            } while(i<length);

            cnv->UCharErrorBufferLength=(int8_t)j;
            *target=t;
            if(offsets!=NULL) {
                *pOffsets=offsets;
            }
            *err=U_BUFFER_OVERFLOW_ERROR;
            return TRUE;
        }

        /* copy the overflow contents to the target */
        *t++=overflow[i++];
        if(offsets!=NULL) {
            *offsets++=-1; /* no source index available for old output */
        }
    }

    /* the overflow buffer is completely copied to the target */
    cnv->UCharErrorBufferLength=0;
    *target=t;
    if(offsets!=NULL) {
        *pOffsets=offsets;
    }
    return FALSE;
}

U_CAPI void U_EXPORT2
ucnv_toUnicode(UConverter *cnv,
               UChar **target, const UChar *targetLimit,
               const char **source, const char *sourceLimit,
               int32_t *offsets,
               UBool flush,
               UErrorCode *err) {
    UConverterToUnicodeArgs args;
    const char *s;
    UChar *t;

    /* check parameters */
    if(err==NULL || U_FAILURE(*err)) {
        return;
    }

    if(cnv==NULL || target==NULL || source==NULL) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    s=*source;
    t=*target;

    if ((const void *)U_MAX_PTR(targetLimit) == (const void *)targetLimit) {
        /*
        Prevent code from going into an infinite loop in case we do hit this
        limit. The limit pointer is expected to be on a UChar * boundary.
        This also prevents the next argument check from failing.
        */
        targetLimit = (const UChar *)(((const char *)targetLimit) - 1);
    }

    /*
     * All these conditions should never happen.
     *
     * 1) Make sure that the limits are >= to the address source or target
     *
     * 2) Make sure that the buffer sizes do not exceed the number range for
     * int32_t because some functions use the size (in units or bytes)
     * rather than comparing pointers, and because offsets are int32_t values.
     *
     * size_t is guaranteed to be unsigned and large enough for the job.
     *
     * Return with an error instead of adjusting the limits because we would
     * not be able to maintain the semantics that either the source must be
     * consumed or the target filled (unless an error occurs).
     * An adjustment would be sourceLimit=t+0x7fffffff; for example.
     *
     * 3) Make sure that the user didn't incorrectly cast a UChar * pointer
     * to a char * pointer and provide an incomplete UChar code unit.
     */
    if (sourceLimit<s || targetLimit<t ||
        ((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s) ||
        ((size_t)(targetLimit-t)>(size_t)0x3fffffff && targetLimit>t) ||
        (((const char *)targetLimit-(const char *)t) & 1) != 0
    ) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    /* output the target overflow buffer */
    if( cnv->UCharErrorBufferLength>0 &&
        ucnv_outputOverflowToUnicode(cnv, target, targetLimit, &offsets, err)
    ) {
        /* U_BUFFER_OVERFLOW_ERROR */
        return;
    }
    /* *target may have moved, therefore stop using t */

    if(!flush && s==sourceLimit && cnv->preToULength>=0) {
        /* the overflow buffer is emptied and there is no new input: we are done */
        return;
    }

    /*
     * Do not simply return with a buffer overflow error if
     * !flush && t==targetLimit
     * because it is possible that the source will not generate any output.
     * For example, the skip callback may be called;
     * it does not output anything.
     */

    /* prepare the converter arguments */
    args.converter=cnv;
    args.flush=flush;
    args.offsets=offsets;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=*target;
    args.targetLimit=targetLimit;
    args.size=sizeof(args);

    _toUnicodeWithCallback(&args, err);

    *source=args.source;
    *target=args.target;
}

/* ucnv_to/fromUChars() ----------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucnv_fromUChars(UConverter *cnv,
                char *dest, int32_t destCapacity,
                const UChar *src, int32_t srcLength,
                UErrorCode *pErrorCode) {
    const UChar *srcLimit;
    char *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return 0;
    }

    if( cnv==NULL ||
        destCapacity<0 || (destCapacity>0 && dest==NULL) ||
        srcLength<-1 || (srcLength!=0 && src==NULL)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    /* initialize */
    ucnv_resetFromUnicode(cnv);
    originalDest=dest;
    if(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    if(srcLength>0) {
        srcLimit=src+srcLength;
        destLimit=dest+destCapacity;

        /* pin the destination limit to U_MAX_PTR; NULL check is for OS/400 */
        if(destLimit<dest || (destLimit==NULL && dest!=NULL)) {
            destLimit=(char *)U_MAX_PTR(dest);
        }

        /* perform the conversion */
        ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, 0, TRUE, pErrorCode);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        if(*pErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            char buffer[1024];

            destLimit=buffer+sizeof(buffer);
            do {
                dest=buffer;
                *pErrorCode=U_ZERO_ERROR;
                ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, 0, TRUE, pErrorCode);
                destLength+=(int32_t)(dest-buffer);
            } while(*pErrorCode==U_BUFFER_OVERFLOW_ERROR);
        }
    } else {
        destLength=0;
    }

    return u_terminateChars(originalDest, destCapacity, destLength, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucnv_toUChars(UConverter *cnv,
              UChar *dest, int32_t destCapacity,
              const char *src, int32_t srcLength,
              UErrorCode *pErrorCode) {
    const char *srcLimit;
    UChar *originalDest, *destLimit;
    int32_t destLength;

    /* check arguments */
    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return 0;
    }

    if( cnv==NULL ||
        destCapacity<0 || (destCapacity>0 && dest==NULL) ||
        srcLength<-1 || (srcLength!=0 && src==NULL))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    /* initialize */
    ucnv_resetToUnicode(cnv);
    originalDest=dest;
    if(srcLength==-1) {
        srcLength=(int32_t)uprv_strlen(src);
    }
    if(srcLength>0) {
        srcLimit=src+srcLength;
        destLimit=dest+destCapacity;

        /* pin the destination limit to U_MAX_PTR; NULL check is for OS/400 */
        if(destLimit<dest || (destLimit==NULL && dest!=NULL)) {
            destLimit=(UChar *)U_MAX_PTR(dest);
        }

        /* perform the conversion */
        ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, 0, TRUE, pErrorCode);
        destLength=(int32_t)(dest-originalDest);

        /* if an overflow occurs, then get the preflighting length */
        if(*pErrorCode==U_BUFFER_OVERFLOW_ERROR)
        {
            UChar buffer[1024];

            destLimit=buffer+UPRV_LENGTHOF(buffer);
            do {
                dest=buffer;
                *pErrorCode=U_ZERO_ERROR;
                ucnv_toUnicode(cnv, &dest, destLimit, &src, srcLimit, 0, TRUE, pErrorCode);
                destLength+=(int32_t)(dest-buffer);
            }
            while(*pErrorCode==U_BUFFER_OVERFLOW_ERROR);
        }
    } else {
        destLength=0;
    }

    return u_terminateUChars(originalDest, destCapacity, destLength, pErrorCode);
}

/* ucnv_getNextUChar() ------------------------------------------------------ */

U_CAPI UChar32 U_EXPORT2
ucnv_getNextUChar(UConverter *cnv,
                  const char **source, const char *sourceLimit,
                  UErrorCode *err) {
    UConverterToUnicodeArgs args;
    UChar buffer[U16_MAX_LENGTH];
    const char *s;
    UChar32 c;
    int32_t i, length;

    /* check parameters */
    if(err==NULL || U_FAILURE(*err)) {
        return 0xffff;
    }

    if(cnv==NULL || source==NULL) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return 0xffff;
    }

    s=*source;
    if(sourceLimit<s) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return 0xffff;
    }

    /*
     * Make sure that the buffer sizes do not exceed the number range for
     * int32_t because some functions use the size (in units or bytes)
     * rather than comparing pointers, and because offsets are int32_t values.
     *
     * size_t is guaranteed to be unsigned and large enough for the job.
     *
     * Return with an error instead of adjusting the limits because we would
     * not be able to maintain the semantics that either the source must be
     * consumed or the target filled (unless an error occurs).
     * An adjustment would be sourceLimit=t+0x7fffffff; for example.
     */
    if(((size_t)(sourceLimit-s)>(size_t)0x7fffffff && sourceLimit>s)) {
        *err=U_ILLEGAL_ARGUMENT_ERROR;
        return 0xffff;
    }

    c=U_SENTINEL;

    /* flush the target overflow buffer */
    if(cnv->UCharErrorBufferLength>0) {
        UChar *overflow;

        overflow=cnv->UCharErrorBuffer;
        i=0;
        length=cnv->UCharErrorBufferLength;
        U16_NEXT(overflow, i, length, c);

        /* move the remaining overflow contents up to the beginning */
        if((cnv->UCharErrorBufferLength=(int8_t)(length-i))>0) {
            uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+i,
                         cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
        }

        if(!U16_IS_LEAD(c) || i<length) {
            return c;
        }
        /*
         * Continue if the overflow buffer contained only a lead surrogate,
         * in case the converter outputs single surrogates from complete
         * input sequences.
         */
    }

    /*
     * flush==TRUE is implied for ucnv_getNextUChar()
     *
     * do not simply return even if s==sourceLimit because the converter may
     * not have seen flush==TRUE before
     */

    /* prepare the converter arguments */
    args.converter=cnv;
    args.flush=TRUE;
    args.offsets=NULL;
    args.source=s;
    args.sourceLimit=sourceLimit;
    args.target=buffer;
    args.targetLimit=buffer+1;
    args.size=sizeof(args);

    if(c<0) {
        /*
         * call the native getNextUChar() implementation if we are
         * at a character boundary (toULength==0)
         *
         * unlike with _toUnicode(), getNextUChar() implementations must set
         * U_TRUNCATED_CHAR_FOUND for truncated input,
         * in addition to setting toULength/toUBytes[]
         */
        if(cnv->toULength==0 && cnv->sharedData->impl->getNextUChar!=NULL) {
            c=cnv->sharedData->impl->getNextUChar(&args, err);
            *source=s=args.source;
            if(*err==U_INDEX_OUTOFBOUNDS_ERROR) {
                /* reset the converter without calling the callback function */
                _reset(cnv, UCNV_RESET_TO_UNICODE, FALSE);
                return 0xffff; /* no output */
            } else if(U_SUCCESS(*err) && c>=0) {
                return c;
            /*
             * else fall through to use _toUnicode() because
             *   UCNV_GET_NEXT_UCHAR_USE_TO_U: the native function did not want to handle it after all
             *   U_FAILURE: call _toUnicode() for callback handling (do not output c)
             */
            }
        }

        /* convert to one UChar in buffer[0], or handle getNextUChar() errors */
        _toUnicodeWithCallback(&args, err);

        if(*err==U_BUFFER_OVERFLOW_ERROR) {
            *err=U_ZERO_ERROR;
        }

        i=0;
        length=(int32_t)(args.target-buffer);
    } else {
        /* write the lead surrogate from the overflow buffer */
        buffer[0]=(UChar)c;
        args.target=buffer+1;
        i=0;
        length=1;
    }

    /* buffer contents starts at i and ends before length */

    if(U_FAILURE(*err)) {
        c=0xffff; /* no output */
    } else if(length==0) {
        /* no input or only state changes */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        /* no need to reset explicitly because _toUnicodeWithCallback() did it */
        c=0xffff; /* no output */
    } else {
        c=buffer[0];
        i=1;
        if(!U16_IS_LEAD(c)) {
            /* consume c=buffer[0], done */
        } else {
            /* got a lead surrogate, see if a trail surrogate follows */
            UChar c2;

            if(cnv->UCharErrorBufferLength>0) {
                /* got overflow output from the conversion */
                if(U16_IS_TRAIL(c2=cnv->UCharErrorBuffer[0])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);

                    /* move the remaining overflow contents up to the beginning */
                    if((--cnv->UCharErrorBufferLength)>0) {
                        uprv_memmove(cnv->UCharErrorBuffer, cnv->UCharErrorBuffer+1,
                                     cnv->UCharErrorBufferLength*U_SIZEOF_UCHAR);
                    }
                } else {
                    /* c is an unpaired lead surrogate, just return it */
                }
            } else if(args.source<sourceLimit) {
                /* convert once more, to buffer[1] */
                args.targetLimit=buffer+2;
                _toUnicodeWithCallback(&args, err);
                if(*err==U_BUFFER_OVERFLOW_ERROR) {
                    *err=U_ZERO_ERROR;
                }

                length=(int32_t)(args.target-buffer);
                if(U_SUCCESS(*err) && length==2 && U16_IS_TRAIL(c2=buffer[1])) {
                    /* got a trail surrogate, too */
                    c=U16_GET_SUPPLEMENTARY(c, c2);
                    i=2;
                }
            }
        }
    }

    /*
     * move leftover output from buffer[i..length[
     * into the beginning of the overflow buffer
     */
    if(i<length) {
        /* move further overflow back */
        int32_t delta=length-i;
        if((length=cnv->UCharErrorBufferLength)>0) {
            uprv_memmove(cnv->UCharErrorBuffer+delta, cnv->UCharErrorBuffer,
                         length*U_SIZEOF_UCHAR);
        }
        cnv->UCharErrorBufferLength=(int8_t)(length+delta);

        cnv->UCharErrorBuffer[0]=buffer[i++];
        if(delta>1) {
            cnv->UCharErrorBuffer[1]=buffer[i];
        }
    }

    *source=args.source;
    return c;
}

#endif
