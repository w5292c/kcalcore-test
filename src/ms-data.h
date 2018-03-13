#ifndef KCALCORE_TEST_DATA_H
#define KCALCORE_TEST_DATA_H

#include <stdint.h>

typedef struct {
  int16_t year;
  int16_t month;
  int16_t dayOfWeek;
  int16_t day;
  int16_t hour;
  int16_t minute;
  int16_t second;
  int16_t milliseconds;
} MsDateTime;

typedef struct {
  int32_t bias;

  wchar_t    standardName[32];
  MsDateTime standardDate;
  int32_t    standardBias;

  wchar_t    daylightName[32];
  MsDateTime daylightDate;
  int32_t    daylightBias;

} MsTimezoneInfo;

#endif /* KCALCORE_TEST_DATA_H */
