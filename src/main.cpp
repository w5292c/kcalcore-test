#include <ms-data.h>

#include <stdio.h>
#include <assert.h>
#include <libical/ical.h>
#include <icaltimezones.h>
#include <QCoreApplication>

//#define LIST_BUILTIN_TIMEZONES
#define CREATE_RU_TIMEZONE
#define CREATE_BERN_TIMEZONE
#define HANDLING_DATES_TIMES

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
#endif /* 0 */

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

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  int result = 0;

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
#endif

  char *const resultStr = icalcomponent_as_ical_string(tzComponent);

  printf("Hello there:\n[%s]\n", resultStr);
  free(resultStr);

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

  return 0;
}
