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

QT_USE_NAMESPACE

namespace {
const wchar_t TheTimezone1Name[] = L"Russian Standard Time";
const wchar_t TheTimezone2Name[] = L"Berlin Standard Time";

MsTimezoneInfo TheMskTimezoneInfo = {
  .bias = -180,
  .standardName = {0},
  .standardDate = { 1601, 1, 0, 1, 0, 0, 0, 0, },
  .standardBias = 0,
  .daylightName = {0},
  .daylightDate = { 1601, 1, 0, 1, 0, 0, 0, 0, },
  .daylightBias = 0,
};

MsTimezoneInfo TheBerlinTimezoneInfo = {
  .bias = -60,
  .standardName = {0},
  .standardDate = { 1601, 10, 0, 1, 3, 0, 0, 0, },
  .standardBias = -60,
  .daylightName = {0},
  .daylightDate = { 1601, 3, 0, 1, 2, 0, 0, 0, },
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

const char TheBerlinTimezoneString[] =
"BEGIN:VTIMEZONE\r\n"
"TZID:Berlin Standard Time\r\n"
"BEGIN:STANDARD\r\n"
"DTSTART:16010101T030000\r\n"
"TZOFFSETFROM:+0200\r\n"
"TZOFFSETTO:+0100\r\n"
"RRULE:FREQ=YEARLY;BYDAY=-1SU;BYMONTH=10\r\n"
"END:STANDARD\r\n"
"BEGIN:DAYLIGHT\r\n"
"DTSTART:16010101T020000\r\n"
"TZOFFSETFROM:+0100\r\n"
"TZOFFSETTO:+0200\r\n"
"RRULE:FREQ=YEARLY;BYDAY=-1SU;BYMONTH=3\r\n"
"END:DAYLIGHT\r\n"
"END:VTIMEZONE\r\n";

const char TheTestTime1Str[] = "20180118T101130Z";
const char TheTestTime2Str[] = "20180718T101130Z";

icalcomponent *parse_ms_timezone(const MsTimezoneInfo *timezoneInfo);
void compare_timezones(const char *expectedValue, const char *value);
void test_time_convertion(const char *timeString, icaltimezone *timezone);
} // namespace {

void test_timezone()
{
  fprintf(stdout, "*********************** Timezone: \"%S\" ***********************\n", TheTimezone1Name);

  /* Initialize the initial structures */
  wcscpy(TheMskTimezoneInfo.standardName, TheTimezone1Name);
  wcscpy(TheMskTimezoneInfo.daylightName, TheTimezone1Name);
  wcscpy(TheBerlinTimezoneInfo.standardName, TheTimezone2Name);
  wcscpy(TheBerlinTimezoneInfo.daylightName, TheTimezone2Name);

  /* Test 'MSK' timezone */
  icalcomponent *timezoneComponent = parse_ms_timezone(&TheMskTimezoneInfo);
  icaltimezone *timezone = icaltimezone_new();
  icaltimezone_set_component(timezone, timezoneComponent);
  /* Test time zonvertion */
  test_time_convertion(TheTestTime1Str, timezone);
  test_time_convertion(TheTestTime2Str, timezone);
  /* Check the generated timezone string */
  char *timezoneString = icalcomponent_as_ical_string_r(timezoneComponent);
  compare_timezones(TheMskTimezoneString, timezoneString);
  free(timezoneString);
  icaltimezone_free(timezone, 1);

  fprintf(stdout, "*********************** Timezone: \"%S\" ***********************\n", TheTimezone2Name);
  /* Test 'MSK' timezone */
  timezoneComponent = parse_ms_timezone(&TheBerlinTimezoneInfo);
  timezone = icaltimezone_new();
  icaltimezone_set_component(timezone, timezoneComponent);
  /* Test time zonvertion */
  test_time_convertion(TheTestTime1Str, timezone);
  test_time_convertion(TheTestTime2Str, timezone);
  /* Check the generated timezone string */
  timezoneString = icalcomponent_as_ical_string_r(timezoneComponent);
  compare_timezones(TheBerlinTimezoneString, timezoneString);
  free(timezoneString);
  icaltimezone_free(timezone, 1);
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

  icalproperty *standardRRule = NULL;
  icalproperty *daylightRRule = NULL;
  if (timezoneInfo->standardBias != timezoneInfo->daylightBias) {
    icalrecurrencetype rrule;

    /* 'RRULE' for 'STANDARD' */
    icalrecurrencetype_clear(&rrule);
    rrule.freq = ICAL_YEARLY_RECURRENCE;
    rrule.interval = 1;
    *rrule.by_month = timezoneInfo->standardDate.month;
    *rrule.by_day = -(timezoneInfo->standardDate.dayOfWeek % 7 + 1) - 8;
    standardRRule = icalproperty_new_rrule(rrule);

    /* 'RRULE' for 'DAYLIGHT' */
    icalrecurrencetype_clear(&rrule);
    rrule.freq = ICAL_YEARLY_RECURRENCE;
    rrule.interval = 1;
    *rrule.by_month = timezoneInfo->daylightDate.month;
    *rrule.by_day = -(timezoneInfo->daylightDate.dayOfWeek % 7 + 1) - 8;
    daylightRRule = icalproperty_new_rrule(rrule);
  }

  icaltimetype standardDtStart;
  memset(&standardDtStart, 0, sizeof (standardDtStart));
  standardDtStart.year = timezoneInfo->standardDate.year;
  standardDtStart.month = 1;
  standardDtStart.day = 1;
  standardDtStart.hour = timezoneInfo->standardDate.hour;
  standardDtStart.minute = timezoneInfo->standardDate.minute;
  standardDtStart.second = timezoneInfo->standardDate.second;

  icaltimetype daylightDtStart;
  memset(&daylightDtStart, 0, sizeof (daylightDtStart));
  daylightDtStart.year = timezoneInfo->daylightDate.year;
  daylightDtStart.month = 1;
  daylightDtStart.day = 1;
  daylightDtStart.hour = timezoneInfo->daylightDate.hour;
  daylightDtStart.minute = timezoneInfo->daylightDate.minute;
  daylightDtStart.second = timezoneInfo->daylightDate.second;

  /* Create the 'STANDARD' component */
  icalcomponent *const standardComponent = icalcomponent_new(ICAL_XSTANDARD_COMPONENT);
  icalcomponent_add_property(standardComponent,
                             icalproperty_new_dtstart(standardDtStart));
  icalcomponent_add_property(standardComponent,
                             icalproperty_new_tzoffsetfrom(-60*(timezoneInfo->bias + timezoneInfo->standardBias)));
  icalcomponent_add_property(standardComponent,
                             icalproperty_new_tzoffsetto(-60*(timezoneInfo->bias + timezoneInfo->daylightBias)));
  if (standardRRule) {
    icalcomponent_add_property(standardComponent, standardRRule);
  }

  /* Create the 'DAYLIGHT' component */
  icalcomponent *const daylightComponent = icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);
  icalcomponent_add_property(daylightComponent,
                             icalproperty_new_dtstart(daylightDtStart));
  icalcomponent_add_property(daylightComponent,
                             icalproperty_new_tzoffsetfrom(-60*(timezoneInfo->bias + timezoneInfo->daylightBias)));
  icalcomponent_add_property(daylightComponent,
                             icalproperty_new_tzoffsetto(-60*(timezoneInfo->bias + timezoneInfo->standardBias)));
  if (daylightRRule) {
    icalcomponent_add_property(daylightComponent, daylightRRule);
  }

  /* Create the 'TIMEZONE' component */
  icalcomponent *const timezoneComponent = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
  icalcomponent_add_property(timezoneComponent, icalproperty_new_tzid(timezoneName.toUtf8()));
  icalcomponent_add_component(timezoneComponent, standardComponent);
  icalcomponent_add_component(timezoneComponent, daylightComponent);

  return timezoneComponent;
}

void compare_timezones(const char *expectedValue, const char *value)
{
  if (!strcmp(expectedValue, value)) {
    printf("\x1b[30;42mTimezones match:\x1b[0m\n[\n%s]\n", value);
  } else {
    printf("\x1b[37;41mTimezones do not match, expected:\x1b[0m\n[\n%s]\n\x1b[37;41mGenerated:\x1b[0m\n[\n%s]\n",
           expectedValue, value);
  }
}

void test_time_convertion(const char *timeString, icaltimezone *timezone)
{
  icaltimetype time1 = icaltime_from_string(timeString);
  char *const time1Str = icaltime_as_ical_string_r(time1);
  icaltimetype time2 = icaltime_convert_to_zone(time1, timezone);
  char *const time2Str = icaltime_as_ical_string_r(time2);
  fprintf(stdout, "Test time (UTC): [%s], converts to (%s): [%s]\n", time1Str, icaltimezone_get_tzid(timezone), time2Str);
  free(time2Str);
  free(time1Str);
}

} // namespace {
