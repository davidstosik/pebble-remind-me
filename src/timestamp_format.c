#include <pebble.h>

#define TIME_STRING_MAX_LENGTH 20
#define SECONDS 1
#define MINUTES 60
#define HOURS   (60*60)
#define DAYS    (24*60*60)

void format_timestamp(time_t timestamp, char * string) {
  time_t now = time(NULL);
  int duration = abs(timestamp - now);
  char * format = "";
  int divider = 0;

  if (duration == 0) {
    strcpy(string, "Now");
    return;
  }
  else if (duration < 60 * SECONDS) {
    format = "%is";
    divider = SECONDS;
  }
  else if (duration < 60 * MINUTES) {
    format = "%imin";
    divider = MINUTES;
  }
  else if (duration < 24 * HOURS) {
    format = "%ih";
    divider = HOURS;
  }
  else if (duration < 7 * DAYS) {
    format = "%i days";
    divider = DAYS;
  }

  if (divider > 0) {
    char full_format[50];
    int value = (duration + divider/2) / divider;
    char * before = "";
    char * after = "";
    if (timestamp > now) {
      before = "In ";
    }
    else {
      after = " ago";
    }
    snprintf(full_format, sizeof(full_format), "%s%s%s", before, format, after);
    snprintf(string, TIME_STRING_MAX_LENGTH, full_format, value);
  }
  else {
    strftime(string, TIME_STRING_MAX_LENGTH, "%k:%M:%S %e-%m-%Y", localtime(&timestamp));
  }
}
