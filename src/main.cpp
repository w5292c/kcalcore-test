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
#include "ms-data.h"
#include "timezone.h"

#include <stdio.h>
#include <assert.h>
#include <libical/ical.h>
//#include <icaltimezones.h>
#include <QCoreApplication>

//#define LIST_BUILTIN_TIMEZONES
//#define MEETING_REPLY
//#define CREATE_RU_TIMEZONE
//#define CREATE_BERN_TIMEZONE
//#define HANDLING_DATES_TIMES
//#define TIMEZONE_TEST
#define TIMEZONE_B64_TEST

#if 0
typedef struct _MSSystemTime {
  qint16 wYear;
  qint16 wMonth;
  qint16 wDayOfWeek;
  qint16 wDay;
  qint16 wHour;
  qint16 wMinute;
  qint16 wSecond;
  qint16 wMilliseconds;
} MSSystemTime;

typedef struct _MSTimeZone {
  long         Bias;
  QString      StandardName;
  MSSystemTime StandardDate;
  long         StandardBias;
  QString      DaylightName;
  MSSystemTime DaylightDate;
  long         DaylightBias;
} MSTimeZone;

static KCalCore::MSTimeZone TestTimezone1 = {
    -60,
    "Europe/Test1", { 0, 10, 0, 5, 3, 0, 0, 0, }, 0,
    "Europe/Test1", { 0, 3, 0, 5, 2, 0, 0, 0, }, -60,
};
static KCalCore::MSTimeZone TestTimezone2 = {
    -180,
    "Europe/Test2", { 1901, 1, 0, 1, 0, 0, 0, 0, }, 0,
    "Europe/Test2", { 1901, 1, 0, 1, 0, 0, 0, 0, }, 0,
};
#endif /* 0 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  int result = 0;

#if 0
  KCalCore::MSTimeZone msTimezone;
  msTimezone.Bias = 180;
  msTimezone.StandardBias = 0;
  msTimezone.DaylightBias = 0;
  msTimezone.StandardName = "RU";
  msTimezone.DaylightName = "RU";
  memset(&msTimezone.StandardDate, 0, sizeof (msTimezone.StandardDate));
  memset(&msTimezone.DaylightDate, 0, sizeof (msTimezone.DaylightDate));

  KCalCore::ICalTimeZoneSource tzSource;
  KCalCore::ICalTimeZone timezone = tzSource.parse(&TestTimezone2);

  icaltimezone *const icaltz = icaltimezone_get_utc_timezone();
//timezone.icalTimezone();
  icalcomponent *const tzComponent = icaltimezone_get_component(icaltz);
#endif /* 0 */

#if 0
  /* Update standard component */
  icalcomponent *standard = icalcomponent_get_first_component(tzComponent, ICAL_XSTANDARD_COMPONENT);
  assert(standard);
  printf("Here it is: %p\n", standard);
  icalcomponent *const standard1 = icalcomponent_get_next_component(tzComponent, ICAL_XSTANDARD_COMPONENT);
  if (standard1) {
    icalcomponent_remove_component(tzComponent, standard1);
  }

  /* Update daylight component */
  icalcomponent *const daylight = icalcomponent_get_first_component(tzComponent, ICAL_XDAYLIGHT_COMPONENT);
  if (daylight) {
    icalcomponent *const daylight1 = icalcomponent_get_next_component(tzComponent, ICAL_XDAYLIGHT_COMPONENT);
    if (daylight1) {
      icalcomponent_remove_component(tzComponent, daylight1);
    }
  }

  char *const resultStr = icalcomponent_as_ical_string(tzComponent);

  printf("Hello there:\n[%s]\n", resultStr);
  free(resultStr);
#endif /* 0 */

#ifdef LIST_BUILTIN_TIMEZONES
  /* Built-in timezones */
  icalarray *const timezones = icaltimezone_get_builtin_timezones();
  for (unsigned int i = 0; i < timezones->num_elements; ++i) {
    icaltimezone *const item = (icaltimezone *)icalarray_element_at(timezones, i);
    icalcomponent *const comp = icaltimezone_get_component(item);
    const char *const location = icaltimezone_get_location(item);
    printf("Timezone (%d): \"%s\", name: \"%s\"\n", i, location,
                                         icaltimezone_get_display_name(item));
    if (comp && 2 <= argc && !strcmp(location, argv[1])) {
      printf("Timezone(%d):[\n%s]\n", i, icalcomponent_as_ical_string(comp));
    }
  }
#endif /* LIST_BUILTIN_TIMEZONES */


#ifdef CREATE_RU_TIMEZONE
/*
Try to generate a timezone like this:
*/
static char TheExpectedRuTimezoneString[] =
"BEGIN:VTIMEZONE\r\n"
"TZID:(UTC+03:00) Moscow\\, St. Petersburg\\, Volgograd\r\n"
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

  printf("Here is the generated timezone:\n");
  icalcomponent *const timezoneComponentRu = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
  /* Add 'TZID' property */
  icalcomponent_add_property(timezoneComponentRu, icalproperty_new_tzid("(UTC+03:00) Moscow, St. Petersburg, Volgograd"));

  /* Add STANDARD component */
  icaltimetype standardDatetimeRu = {
    1601, 1, 1,
    0, 0, 0,
    0, 0, 0, NULL
  };
  icalcomponent *const standardComponentRu = icalcomponent_new(ICAL_XSTANDARD_COMPONENT);
  icalcomponent_add_property(standardComponentRu, icalproperty_new_dtstart(standardDatetimeRu));
  icalcomponent_add_property(standardComponentRu, icalproperty_new_tzoffsetfrom(180*60));
  icalcomponent_add_property(standardComponentRu, icalproperty_new_tzoffsetto(180*60));
  icalcomponent_add_component(timezoneComponentRu, standardComponentRu);

  /* Add DAYLIGHT component */
  icaltimetype daylightDatetimeRu = {
    1601, 1, 1,
    0, 0, 0,
    0, 0, 0, NULL
  };
  icalcomponent *const daylightComponentRu = icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);
  icalcomponent_add_property(daylightComponentRu, icalproperty_new_dtstart(daylightDatetimeRu));
  icalcomponent_add_property(daylightComponentRu, icalproperty_new_tzoffsetfrom(180*60));
  icalcomponent_add_property(daylightComponentRu, icalproperty_new_tzoffsetto(180*60));
  icalcomponent_add_component(timezoneComponentRu, daylightComponentRu);

  char *const timezoneStringRu = icalcomponent_as_ical_string(timezoneComponentRu);
  printf("[\n%s]\n", timezoneStringRu);
  printf("Expected:\n[\n%s]\n", TheExpectedRuTimezoneString);

  assert(!strcmp(TheExpectedRuTimezoneString, timezoneStringRu));
  free(timezoneStringRu);
  printf("Timezones match!!!\n");
#endif /* CREATE_RU_TIMEZONE */

#ifdef CREATE_BERN_TIMEZONE
static const char TheExpectedBernTimezoneString[] = "BEGIN:VTIMEZONE\r\n"
"TZID:(UTC+01:00) Amsterdam\\, Berlin\\, Bern\\, Rome\\, Stockholm\\, Vienna\r\n"
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
// Interval is '1' on default, it does not appear in libical, comment it out for now
//"RRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=10\r\n"
//"RRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=3\r\n"

  printf("Here is the generated timezone:\n");
  icalcomponent *const timezoneComponentBern = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
  /* Add 'TZID' property */
  icalcomponent_add_property(timezoneComponentBern, icalproperty_new_tzid("(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna"));

  /* Add STANDARD component */
  icaltimetype standardDatetimeBern = {
    1601, 1, 1,
    3, 0, 0,
    0, 0, 0, NULL
  };
  const icalrecurrencetype stdBernRRule = icalrecurrencetype_from_string("FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=10");
  icalcomponent *const standardComponentBern = icalcomponent_new(ICAL_XSTANDARD_COMPONENT);
  icalcomponent_add_property(standardComponentBern, icalproperty_new_dtstart(standardDatetimeBern));
  icalcomponent_add_property(standardComponentBern, icalproperty_new_tzoffsetfrom(120*60));
  icalcomponent_add_property(standardComponentBern, icalproperty_new_tzoffsetto(60*60));
  icalcomponent_add_property(standardComponentBern, icalproperty_new_rrule(stdBernRRule));
  icalcomponent_add_component(timezoneComponentBern, standardComponentBern);

  /* Add DAYLIGHT component */
  icaltimetype daylightDatetimeBern = {
    1601, 1, 1,
    2, 0, 0,
    0, 0, 0, NULL
  };
  const icalrecurrencetype dlBernRRule = icalrecurrencetype_from_string("FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=3");
  icalcomponent *const daylightComponentBern = icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);
  icalcomponent_add_property(daylightComponentBern, icalproperty_new_dtstart(daylightDatetimeBern));
  icalcomponent_add_property(daylightComponentBern, icalproperty_new_tzoffsetfrom(60*60));
  icalcomponent_add_property(daylightComponentBern, icalproperty_new_tzoffsetto(120*60));
  icalcomponent_add_property(daylightComponentBern, icalproperty_new_rrule(dlBernRRule));
  icalcomponent_add_component(timezoneComponentBern, daylightComponentBern);

  char *const timezoneStringBern = icalcomponent_as_ical_string(timezoneComponentBern);
  printf("[\n%s]\n", timezoneStringBern);
  printf("Expected:\n[\n%s]\n", TheExpectedBernTimezoneString);

  assert(!strcmp(TheExpectedBernTimezoneString, timezoneStringBern));
  free(timezoneStringBern);
  printf("Timezones match!!!\n");

#endif /* CREATE_BERN_TIMEZONE */

#ifdef HANDLING_DATES_TIMES
  icaltimezone *const timezoneForTime = icaltimezone_new();
  result = icaltimezone_set_component(timezoneForTime, timezoneComponentRu);
  assert(1 == result);
  icaltimetype timeForTime = icaltime_from_string("20180308T123000"/*, timezoneForTime*/);
  icaltimetype timeForTimeTz = icaltime_set_timezone(&timeForTime, timezoneForTime);
  icalproperty *const dtStartProp = icalproperty_new_dtstart(timeForTimeTz);
  const char *const timeStr = icalproperty_as_ical_string(dtStartProp);
  printf("Here is the formatted string: [%s]\n", timeStr);
  const char *const tzid = icaltimezone_get_tzid((icaltimezone *)icaltime_get_timezone(timeForTimeTz));
  printf("TZID: [%s]\n", tzid);

  /* Playing with properties/parameters */
  icalproperty *const testProp = icalproperty_new(ICAL_DTSTART_PROPERTY);
  icalproperty_set_value_from_string(testProp, "20180308T143000", icalvalue_kind_to_string(ICAL_DATETIME_VALUE));
  icalproperty_set_parameter_from_string(testProp, "TZID", tzid);

  const char *const testPropStr = icalproperty_as_ical_string(testProp);
  printf("Here is a test property: [%s]\n", testPropStr);

  icaltimezone_free(timezoneForTime, true);
#endif /* HANDLING_DATES_TIMES */

#ifdef MEETING_REPLY
  printf("************************ CREATE REPLY ************************\n");
static const char TheRequest[] = "BEGIN:VCALENDAR\r\n"
"METHOD:REQUEST\r\n"
"PRODID:Microsoft Exchange Server 2010\r\n"
"VERSION:2.0\r\n"
"BEGIN:VTIMEZONE\r\n"
"TZID:(UTC+01:00) Amsterdam\\, Berlin\\, Bern\\, Rome\\, Stockholm\\, Vienna\r\n"
"BEGIN:STANDARD\r\n"
"DTSTART:16010101T030000\r\n"
"TZOFFSETFROM:+0200\r\n"
"TZOFFSETTO:+0100\r\n"
"RRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=10\r\n"
"END:STANDARD\r\n"
"BEGIN:DAYLIGHT\r\n"
"DTSTART:16010101T020000\r\n"
"TZOFFSETFROM:+0100\r\n"
"TZOFFSETTO:+0200\r\n"
"RRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=3\r\n"
"END:DAYLIGHT\r\n"
"END:VTIMEZONE\r\n"
"BEGIN:VEVENT\r\n"
"ORGANIZER;CN=Chumakov Alexander:MAILTO:w5292c@outlook.com\r\n"
"ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=NEEDS-ACTION;RSVP=TRUE;CN=Chumakov A\r\n"
" lexander:MAILTO:w5292c@outlook.com\r\n"
"ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=NEEDS-ACTION;RSVP=TRUE;CN=Челов\r\n"
" ек Тестовый:MAILTO:w5292c.ex2@gmail.com\r\n"
"ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=NEEDS-ACTION;RSVP=TRUE;CN=alexander.\r\n"
" chumakov@harman.com:MAILTO:alexander.chumakov@harman.com\r\n"
"DESCRIPTION;LANGUAGE=ru-RU:\\n\r\n"
"UID:040000008200E00074C5B7101A82E0080000000030FE6485D7B6D301000000000000000\r\n"
" 0100000004047422450593442A34B9832B03B232D\r\n"
"SUMMARY;LANGUAGE=ru-RU:London time event A1\r\n"
"DTSTART;TZID=\"(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna\":2\r\n"
" 0180401T190000\r\n"
"DTEND;TZID=\"(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna\":201\r\n"
" 80401T193000\r\n"
"CLASS:PUBLIC\r\n"
"PRIORITY:5\r\n"
"DTSTAMP:20180308T121809Z\r\n"
"TRANSP:OPAQUE\r\n"
"STATUS:CONFIRMED\r\n"
"SEQUENCE:0\r\n"
"LOCATION;LANGUAGE=ru-RU:Location A1\r\n"
"X-MICROSOFT-CDO-APPT-SEQUENCE:0\r\n"
"X-MICROSOFT-CDO-OWNERAPPTID:2116240176\r\n"
"X-MICROSOFT-CDO-BUSYSTATUS:TENTATIVE\r\n"
"X-MICROSOFT-CDO-INTENDEDSTATUS:BUSY\r\n"
"X-MICROSOFT-CDO-ALLDAYEVENT:FALSE\r\n"
"X-MICROSOFT-CDO-IMPORTANCE:1\r\n"
"X-MICROSOFT-CDO-INSTTYPE:0\r\n"
"X-MICROSOFT-DONOTFORWARDMEETING:FALSE\r\n"
"X-MICROSOFT-DISALLOW-COUNTER:FALSE\r\n"
"X-MICROSOFT-LOCATIONDISPLAYNAME:Location A1\r\n"
"X-MICROSOFT-LOCATIONSOURCE:None\r\n"
"BEGIN:VALARM\r\n"
"DESCRIPTION:REMINDER\r\n"
"TRIGGER;RELATED=START:-PT15M\r\n"
"ACTION:DISPLAY\r\n"
"END:VALARM\r\n"
"END:VEVENT\r\n"
"END:VCALENDAR\r\n";
  static const char myEmail[] = "w5292c.ex2@gmail.com";

  /* EVENT elements */
  icalproperty *uidProperty = NULL;
  icalproperty *dtendProperty = NULL;
  icalproperty *transpProperty = NULL;
  icalproperty *dtstartProperty = NULL;
  icalproperty *dtstampProperty = NULL;
  icalproperty *summaryProperty = NULL;
  icalproperty *locationProperty = NULL;
  icalproperty *attendeeProperty = NULL;
  icalproperty *organizerProperty = NULL;
  QList<icalproperty *> xpropertiesList;
  /* VCALENDAR elements */
  icalcomponent *replyComponent = NULL;
  icalcomponent *replyEventComponent = NULL;
  const icalproperty *prodidProperty = NULL;
  const icalproperty *methodProperty = NULL;
  const icalproperty *versionProperty = NULL;
  icalcomponent *eventComponent = NULL;
  icalcomponent *timezoneComponent = NULL;
  icalcomponent *const requestComponent = icalparser_parse_string(TheRequest);
  assert(requestComponent);
  icalcomponent *currentComponent = NULL;
  for (currentComponent = icalcomponent_get_first_component(requestComponent, ICAL_ANY_COMPONENT);
       currentComponent;
       currentComponent = icalcomponent_get_next_component(requestComponent, ICAL_ANY_COMPONENT)) {
    switch (icalcomponent_isa(currentComponent)) {
    case ICAL_VEVENT_COMPONENT:
      eventComponent = currentComponent;
      break;
    case ICAL_VTIMEZONE_COMPONENT:
      timezoneComponent = currentComponent;
      break;
    default:
      printf("Unknown component: \"%s\"\n", icalcomponent_kind_to_string(icalcomponent_isa(currentComponent)));
      break;
    }
  }
  icalproperty *currentProperty = NULL;
  for (currentProperty = icalcomponent_get_first_property(requestComponent, ICAL_ANY_PROPERTY);
       currentProperty;
       currentProperty = icalcomponent_get_next_property(requestComponent, ICAL_ANY_PROPERTY)) {
    switch (icalproperty_isa(currentProperty)) {
    case ICAL_PRODID_PROPERTY:
      prodidProperty = currentProperty;
      break;
    case ICAL_METHOD_PROPERTY:
      methodProperty = currentProperty;
      break;
    case ICAL_VERSION_PROPERTY:
      versionProperty = currentProperty;
      break;
    default:
      printf("Unknown property in request: \"%s\"\n", icalproperty_kind_to_string(icalproperty_isa(currentProperty)));
      break;
    }
  }

  if (methodProperty) {
    const char *const methodStr = icalproperty_get_value_as_string(methodProperty);
    if (!strcmp(methodStr, "REQUEST")) {
      printf("Info: method in request: \"%s\"\n", methodStr);
    } else {
      printf("Fatal: wrong method in request: \"%s\"\n", methodStr);
      goto cleanup;
    }
  } else {
    printf("Fatal: no method element in request\n");
    goto cleanup;
  }
  if (versionProperty) {
    printf("Info: Request ICAL version: \"%s\"\n", icalproperty_get_value_as_string(versionProperty));
  } else {
    printf("Warning: no version element in request\n");
  }
  if (prodidProperty) {
    printf ("Info: Our requestor: \"%s\"\n", icalproperty_get_value_as_string(prodidProperty));
  }
  if (timezoneComponent) {
    printf("Info: timezone in request: [%s]\n", icalcomponent_as_ical_string((icalcomponent *)timezoneComponent));
  }

  /* Parse EVENT component */
  if (!eventComponent) {
    fprintf(stderr, "Fatal: no EVENT component\n");
    goto cleanup;
  }
  /* Parsing the event comonent */
  currentProperty = NULL;
  for (currentProperty = icalcomponent_get_first_property(eventComponent, ICAL_ANY_PROPERTY);
       currentProperty;
       currentProperty = icalcomponent_get_next_property(eventComponent, ICAL_ANY_PROPERTY)) {
    const icalproperty_kind kind = icalproperty_isa(currentProperty);
    switch (kind) {
    case ICAL_UID_PROPERTY:
      uidProperty = currentProperty;
      break;
    case ICAL_DTSTART_PROPERTY:
      dtstartProperty = currentProperty;
      break;
    case ICAL_DTEND_PROPERTY:
      dtendProperty = currentProperty;
      break;
    case ICAL_DTSTAMP_PROPERTY:
      dtstampProperty = currentProperty;
      break;
    case ICAL_ORGANIZER_PROPERTY:
      organizerProperty = currentProperty;
      break;
    case ICAL_LOCATION_PROPERTY:
      locationProperty = currentProperty;
      break;
    case ICAL_SUMMARY_PROPERTY:
      summaryProperty = currentProperty;
      break;
    case ICAL_TRANSP_PROPERTY:
      transpProperty = currentProperty;
      break;
    case ICAL_ATTENDEE_PROPERTY: {
      const char emailPrefix[] = "mailto:";
      const char *const value = icalproperty_get_value_as_string(currentProperty);
      printf("Attendee: [%s]\n", value);
      if (!strncasecmp(value, emailPrefix, sizeof (emailPrefix) - 1) &&
          !strcasecmp(value + sizeof (emailPrefix) - 1, myEmail)) {
        attendeeProperty = currentProperty;
      }
      }
      break;
    case ICAL_X_PROPERTY:
      xpropertiesList.append(currentProperty);
      printf("Here goes new event xproperty: [%s]\n", icalproperty_get_x_name(currentProperty));
      break;
    default:
      printf("Here goes new event property: [%s]\n", icalproperty_kind_to_string(icalproperty_isa(currentProperty)));
      break;
    }
  }
  for (currentComponent = icalcomponent_get_first_component(eventComponent, ICAL_ANY_COMPONENT);
       currentComponent;
       currentComponent = icalcomponent_get_next_component(eventComponent, ICAL_ANY_COMPONENT)) {
    printf("Here goes new event component: [%s]\n", icalcomponent_kind_to_string(icalcomponent_isa(currentComponent)));
  }

  /* Create the reply */
  do {
    /* Create the EVENT component */
    replyEventComponent = icalcomponent_new(ICAL_VEVENT_COMPONENT);
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(dtstartProperty));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(dtendProperty));
    icaltimetype timeNowUtc = icaltime_current_time_with_zone(icaltimezone_get_utc_timezone());
    icalcomponent_add_property(replyEventComponent, icalproperty_new_dtstamp(timeNowUtc));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(organizerProperty));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(uidProperty));
    {
    /** @todo Add attendee property here */
    icalproperty *const me = icalproperty_new_clone(attendeeProperty);
    icalproperty_remove_parameter(me, ICAL_RSVP_PARAMETER);
    icalproperty_remove_parameter(me, ICAL_PARTSTAT_PARAMETER);
    icalproperty_add_parameter(me, icalparameter_new_partstat(ICAL_PARTSTAT_ACCEPTED));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(me));
    }
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(dtstampProperty));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_lastmodified(timeNowUtc));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(locationProperty));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_sequence(0));
    icalcomponent_set_status(replyEventComponent, ICAL_STATUS_CONFIRMED);
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(summaryProperty));
    icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(transpProperty));
    foreach (icalproperty *xprop, xpropertiesList) {
      icalcomponent_add_property(replyEventComponent, icalproperty_new_clone(xprop));
    }

    /* Create the high-level VCALENDAR component */
    replyComponent = icalcomponent_new(ICAL_VCALENDAR_COMPONENT);
    icalcomponent_add_property(replyComponent, icalproperty_new_method(ICAL_METHOD_REPLY));
    icalcomponent_add_property(replyComponent, icalproperty_new_prodid("Sailfish OS, version 1.0"));
    icalcomponent_add_property(replyComponent, icalproperty_new_version("2.0"));
    icalcomponent_add_component(replyComponent, icalcomponent_new_clone(timezoneComponent));
    icalcomponent_add_component(replyComponent, replyEventComponent);

    printf("******************************************* REPLY *******************************************\n");
    char *const replyStr = icalcomponent_as_ical_string(replyComponent);
    printf("Info: generated reply:[%s]\n", replyStr);
    free(replyStr);
  } while (false);

cleanup:
  if (replyComponent) {
    icalcomponent_free(replyComponent);
  }
  icalcomponent_free(requestComponent);
#endif /* MEETING_REPLY */

#ifdef TIMEZONE_TEST
  /* Timezone test */
  test_timezone();
#endif /* TIMEZONE_TEST */

#ifdef TIMEZONE_B64_TEST
  test_timezones_b64();
#endif /* TIMEZONE_B64_TEST */

  return result;
}
