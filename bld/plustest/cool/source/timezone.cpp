// Created: JAM 08/24/92 -- created to move large static array from header

#include <cool/timezone.h>

const char* const tz_table[] = {
  "Unknown Time Zone",                  // Unknown time zone
  "US/Eastern",                         // Eastern time zone, USA
  "US/Central",                         // Central time zone, USA
  "US/Mountain",                        // Mountain time zone, USA
  "US/Pacific",                         // Pacific time zone, USA
  "US/Pacific-New",                     // Pacific time zone, USA with DST
  "US/Yukon",                           // Yukon time zone, USA
  "US/East-Indiana",                    // Estern time zone, USA with no DST
  "US/Arizona",                         // Mountain time zone, USA with no DST
  "US/Hawaii",                          // Hawaii
  "Canada/Newfoundland",                // Newfoundland
  "Canada/Atlantic",                    // Atlantic time zone, Canada
  "Canada/Eastern",                     // Eastern time zone, Canada
  "Canada/Central",                     // Central time zone, Canada
  "Canada/East-Saskatchewan",           // Central time zone, Canada with no DST
  "Canada/Mountain",                    // Mountain time zone, Canada
  "Canada/Pacific",                     // Pacific time zone, Canada
  "Canada/Yukon",                       // Yukon time zone, Canada
  "GB-Eire",                            // Great Britain and Eire (GMT)
  "WET",                                // Western Europe time
  "Iceland",                            // Iceland
  "MET",                                // Middle Europe time
  "Poland",                             // Poland
  "EET",                                // Eastern Europe time
  "Turkey",                             // Turkey
  "W-SU",                               // Western Soviet Union
  "PRC",                                // People's Republic of China
  "Korea",                              // Republic of Korea
  "Japan",                              // Japan
  "Singapore",                          // Singapore
  "Hongkong",                           // Hongkong
  "ROC",                                // Republic of China
  "Australia/Tasmania",                 // Tasmainia, Australia
  "Australia/Queensland",               // Queensland, Australia
  "Australia/North",                    // Northern Territory, Australia
  "Australia/West",                     // Western Australia
  "Australia/South",                    // South Australia
  "Australia/Victoria",                 // Victoria, Australia
  "Australia/NSW",                      // New South Wales, Australia
  "NZ"                                  // New Zealand
  };
