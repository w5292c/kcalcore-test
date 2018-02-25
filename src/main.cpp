#include <stdio.h>
#include <assert.h>
#include <libical/ical.h>
#include <icaltimezones.h>
#include <QCoreApplication>

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
    "Europe/Test2", { 0, 0, 0, 0, 0, 0, 0, 0, }, 0,
    "Europe/Test2", { 0, 0, 0, 0, 0, 0, 0, 0, }, 0,
};

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  KCalCore::MSTimeZone msTimezone;
  msTimezone.Bias = 180;
  msTimezone.StandardBias = 0;
  msTimezone.DaylightBias = 0;
  msTimezone.StandardName = "RU";
  msTimezone.DaylightName = "RU";
  memset(&msTimezone.StandardDate, 0, sizeof (msTimezone.StandardDate));
  memset(&msTimezone.DaylightDate, 0, sizeof (msTimezone.DaylightDate));

  KCalCore::ICalTimeZoneSource tzSource;
  KCalCore::ICalTimeZone timezone = tzSource.parse(&TestTimezone1);

  icaltimezone *const icaltz = timezone.icalTimezone();
  icalcomponent *const tzComponent = icaltimezone_get_component(icaltz);

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

  char *const result = icalcomponent_as_ical_string(tzComponent);

  printf("Hello there:\n[%s]\n", result);
  free(result);
  return 0;
}
