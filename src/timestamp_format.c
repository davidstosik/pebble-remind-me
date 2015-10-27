#include <pebble.h>

#define TIME_STRING_MAX_LENGTH 20
#define SECOND 1
#define MINUTE 60
#define HOUR   (60*60)
#define DAY    (24*60*60)

void format_timestamp(time_t timestamp, char * string) {
  time_t now = time(NULL);
  int duration = abs(timestamp - now);
  char * format = "";
  int divider = 0;

  if (duration == 0) {
    strcpy(string, "Just now");
    return;
  }
  else if (duration < 60 * SECOND) {
    format = "%i seconds";
    divider = SECOND;
  }
  else if (duration < 60 * MINUTE) {
    format = "%i minutes";
    divider = MINUTE;
  }
  else if (duration < 24 * HOUR) {
    format = "%i hours";
    divider = HOUR;
  }
  else if (duration < 7 * DAY) {
    format = "%i days";
    divider = DAY;
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
    strftime(string, TIME_STRING_MAX_LENGTH, "%b %e %Y", localtime(&timestamp));
  }
}
