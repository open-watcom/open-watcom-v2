//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
//
// Initial implementation: 4/11/89 MBN
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/24/92 -- made static country_name array global and moved
//                          to Country.c
//
// This  file  contains an  enum  definition for  type country.  It  specifies
// symbolic constants to  be used as  country types.  Note  that C++  does not
// distinguish between an enum type  and an integer,  so function arguments of
// type `country' are really just `int' types.
//

#ifndef COUNTRYH                // If we have not yet defined the country type,
#define COUNTRYH                // Indicate that class country has been included

enum country {  
  UNKNOWN_COUNTRY,                              // Unknown country
  UNITED_STATES,                                // 29-03-1989 17:35:00.00
  FRENCH_CANADIAN,                              // 1989-29-03 17:35:00,00
  LATIN_AMERICA,                                // 03/29/1989 17:35:00.00
  NETHERLANDS,                                  // 03-29-1989 17:35:00,00
  BELGIUM,                                      // 03/29/1989 17:35:00,00
  FRANCE,                                       // 03/29/1989 17:35:00,00
  SPAIN,                                        // 03/29/1989 17:35:00,00
  ITALY,                                        // 03/29/1989 17:35:00,00
  SWITZERLAND,                                  // 03.29.1989 17.35.00.00
  UNITED_KINGDOM,                               // 03-29-1989 17:35:00.00
  DENMARK,                                      // 03/29/1989 17:35:00,00
  SWEDEN,                                       // 1989-29-03 17.35.00,00
  NORWAY,                                       // 03/29/1989 17:35:00,00
  GERMANY,                                      // 03.29.1989 17.35.00,00
  PORTUGAL,                                     // 03/29/1989 17:35:00,00
  FINLAND,                                      // 03.29.1989 17.35.00,00
  ARABIC_COUNTRIES,                             // 03/29/1989 17:35:00,00
  ISRAEL                                        // 03 29 1989 17:35:00.00
};

extern const char* const country_names[];               // ASCII country names

#endif                                          // End #ifdef of COUNTRYH
