#ifndef __LANGUAGE_H
#define __LANGUAGE_H

/*
 * language.h
 * Include file for WatTCP foreign language translation facility.
 * Based on idea from PGP, but totally rewritten (using flex)
 *
 * Strings with _LANG() around them are found by the `mklang' tool and
 * put into a text file to be translated into foreign language at run-time.
 */

#if defined(USE_LANGUAGE)
  extern void  lang_init (const char *value);
  extern const char *_LANG (const char *str);
#else
  #define _LANG(str)  str
#endif

/*
 * __LANG() must be used for array string constants. This macro is used by
 * the `mklang' to generate an entry in the language database.
 */
#define __LANG(str)  str

#endif
