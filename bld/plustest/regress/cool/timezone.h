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
// Created: MBN 04/11/89 -- Initial design and implementation
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/24/92 -- made static array global and moved to .c
//
// This file contains an  enum  definition  for  type timezone.  It  specifies
// symbolic  constants to be used as  timezone  types.  Note that C++ does not
// distinguish between an  enum type and  an integer, so function arguments of
// type `timezone' are really just `int' types.

#ifndef TIMEZONEH               // If we have not yet defined the timezone type,
#define TIMEZONEH               // Indicate that timezone has been included

enum time_zone {                        // Time zone specification
  UNKNOWN_TIME_ZONE,                    // Unknown time zone
  US_EASTERN,                           // Eastern time zone, USA
  US_CENTRAL,                           // Central time zone, USA
  US_MOUNTAIN,                          // Mountain time zone, USA
  US_PACIFIC,                           // Pacific time zone, USA
  US_PACIFIC_NEW,                       // Pacific time zone, USA with DST
  US_YUKON,                             // Yukon time zone, USA
  US_EAST_INDIANA,                      // Estern time zone, USA with no DST
  US_ARIZONA,                           // Mountain time zone, USA with no DST
  US_HAWAII,                            // Hawaii
  CANADA_NEWFOUNDLAND,                  // Newfoundland
  CANADA_ATLANTIC,                      // Atlantic time zone, Canada
  CANADA_EASTERN,                       // Eastern time zone, Canada
  CANADA_CENTRAL,                       // Central time zone, Canada
  CANADA_EAST_SASKATCHEWAN,             // Central time zone, Canada with no DST
  CANADA_MOUNTAIN,                      // Mountain time zone, Canada
  CANADA_PACIFIC,                       // Pacific time zone, Canada
  CANADA_YUKON,                         // Yukon time zone, Canada
  GB_EIRE,                              // Great Britain and Eire
  WET,                                  // Western Europe time
  ICELAND,                              // Iceland
  MET,                                  // Middle Europe time
  POLAND,                               // Poland
  EET,                                  // Eastern Europe time
  TURKEY,                               // Turkey
  W_SU,                                 // Western Soviet Union
  PRC,                                  // People's Republic of China
  KOREA,                                // Republic of Korea
  JAPAN,                                // Japan
  SINGAPORE,                            // Singapore
  HONGKONG,                             // Hongkong
  ROC,                                  // Republic of China
  AUSTRALIA_TASMANIA,                   // Tasmainia, Australia
  AUSTRALIA_QUEENSLAND,                 // Queensland, Australia
  AUSTRALIA_NORTH,                      // Northern Territory, Australia
  AUSTRALIA_WEST,                       // Western Australia
  AUSTRALIA_SOUTH,                      // South Australia
  AUSTRALIA_VICTORIA,                   // Victoria, Australia
  AUSTRALIA_NSW,                        // New South Wales, Australia
  NZ                                    // New Zealand
};

extern const char* const tz_table[];

#endif                                          // End #ifdef of TIMEZONEH
