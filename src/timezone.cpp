/*
 * MIT License
 *
 * Copyright (c) 2018 Alexander Chumakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "timezone.h"

#include "ms-data.h"

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <QString>
#include <libical/ical.h>

/*typedef struct {
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

  int16_t    standardName[32];
  MsDateTime standardDate;
  int32_t    standardBias;

  int16_t    daylightName[32];
  MsDateTime daylightDate;
  int32_t    daylightBias;

} MsTimezoneInfo;*/

QT_USE_NAMESPACE

namespace {
const wchar_t TheTimezoneName[] = L"Russian Standard Time";

MsTimezoneInfo TheMskTimezoneInfo = {
  .bias = -180,
  .standardName = {0},
  .standardDate = { 1601, 1, 0, 1, 0, 0, 0, 0, },
  .standardBias = 0,
  .daylightName = {0},
  .daylightDate = { 1601, 1, 0, 1, 0, 0, 0, 0, },
  .daylightBias = 0,
};

const char TheMskTimezoneString[] =
"BEGIN:VTIMEZONE\r\n"
"TZID:Russian Standard Time\r\n"
"BEGIN:STANDARD\r\n"
"DTSTART:16010101T000000\r\n"
"TZOFFSETFROM:+0300\r\n"
"TZOFFSETTO:+0300\r\n"
"END:STANDARD\r\n"
"BEGIN:DAYLIGHT\r\n"
"DTSTART:16010101T000000\r\n"
"TZOFFSETFROM:+0300\r\n"
"TZOFFSETTO:+0300\r\n"
"END:DAYLIGHT\r\n"
"END:VTIMEZONE\r\n";

icalcomponent *parse_ms_timezone(const MsTimezoneInfo *timezoneInfo);
} // namespace {

void test_timezone()
{
  /* Initialize the initial structures */
  wcscpy(TheMskTimezoneInfo.standardName, TheTimezoneName);
  wcscpy(TheMskTimezoneInfo.daylightName, TheTimezoneName);

  /* Test 'MSK' timezone */
  icalcomponent *const timezoneComponent = parse_ms_timezone(&TheMskTimezoneInfo);
  char *const timezoneString = icalcomponent_as_ical_string_r(timezoneComponent);

  if (!strcmp(TheMskTimezoneString, timezoneString)) {
    printf("\x1b[30;42mTimezones match:\x1b[0m\n[\n%s]\n", timezoneString);
  } else {
    printf("\x1b[37;41mTimezones do not match, expected:\x1b[0m\n[\n%s]\n\x1b[37;41mGenerated:\x1b[0m\n[\n%s]\n",
           TheMskTimezoneString, timezoneString);
  }
  free(timezoneString);
}

namespace {

icalcomponent *parse_ms_timezone(const MsTimezoneInfo *timezoneInfo)
{
  /* Check argument(s) */
  if (!timezoneInfo) {
    /* No required arguments */
    fprintf(stderr, "Error: no timezone information\n");
    return NULL;
  }

  /* Start handling timezones */
  QString timezoneName = QString::fromWCharArray(timezoneInfo->standardName);
  if (timezoneName.isEmpty()) {
    timezoneName = QString::fromWCharArray(timezoneInfo->daylightName);
  }
  if (timezoneName.isEmpty()) {
    /* Timezone name does not exist */
    fprintf(stderr, "Error: no timezone name\n");
    return NULL;
  }

  icaltimetype standardDtStart;
  memset(&standardDtStart, 0, sizeof (standardDtStart));
  standardDtStart.year = TheMskTimezoneInfo.standardDate.year;
  standardDtStart.month = TheMskTimezoneInfo.standardDate.month;
  standardDtStart.day = TheMskTimezoneInfo.standardDate.day;
  standardDtStart.hour = TheMskTimezoneInfo.standardDate.hour;
  standardDtStart.minute = TheMskTimezoneInfo.standardDate.minute;
  standardDtStart.second = TheMskTimezoneInfo.standardDate.second;

  icaltimetype daylightDtStart;
  memset(&daylightDtStart, 0, sizeof (daylightDtStart));
  daylightDtStart.year = TheMskTimezoneInfo.daylightDate.year;
  daylightDtStart.month = TheMskTimezoneInfo.daylightDate.month;
  daylightDtStart.day = TheMskTimezoneInfo.daylightDate.day;
  daylightDtStart.hour = TheMskTimezoneInfo.daylightDate.hour;
  daylightDtStart.minute = TheMskTimezoneInfo.daylightDate.minute;
  daylightDtStart.second = TheMskTimezoneInfo.daylightDate.second;

  /* Create the 'STANDARD' component */
  icalcomponent *const standardComponent = icalcomponent_new(ICAL_XSTANDARD_COMPONENT);
  icalcomponent_add_property(standardComponent,
                             icalproperty_new_dtstart(standardDtStart));
  icalcomponent_add_property(standardComponent,
                             icalproperty_new_tzoffsetfrom(-60*(TheMskTimezoneInfo.bias + TheMskTimezoneInfo.daylightBias)));
  icalcomponent_add_property(standardComponent,
                             icalproperty_new_tzoffsetto(-60*(TheMskTimezoneInfo.bias + TheMskTimezoneInfo.standardBias)));

  /* Create the 'DAYLIGHT' component */
  icalcomponent *const daylightComponent = icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);
  icalcomponent_add_property(daylightComponent,
                             icalproperty_new_dtstart(daylightDtStart));
  icalcomponent_add_property(daylightComponent,
                             icalproperty_new_tzoffsetfrom(-60*(TheMskTimezoneInfo.bias + TheMskTimezoneInfo.standardBias)));
  icalcomponent_add_property(daylightComponent,
                             icalproperty_new_tzoffsetto(-60*(TheMskTimezoneInfo.bias + TheMskTimezoneInfo.daylightBias)));

  /* Create the 'TIMEZONE' component */
  icalcomponent *const timezoneComponent = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
  icalcomponent_add_property(timezoneComponent, icalproperty_new_tzid(timezoneName.toUtf8()));
  icalcomponent_add_component(timezoneComponent, standardComponent);
  icalcomponent_add_component(timezoneComponent, daylightComponent);

  return timezoneComponent;
}

} // namespace {