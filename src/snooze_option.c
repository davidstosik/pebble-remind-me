#include <pebble.h>
#include "constants.h"
#include "snooze_option.h"
  
static SnoozeOption snooze_options[SNOOZE_OPT_N];

SnoozeOption* get_snooze_options(time_t now) {
  static bool set = false;
  if(!set) {
    strcpy(snooze_options[NEVER].label, "Never");
    strcpy(snooze_options[TEN_MIN].label, "10 min.");
    strcpy(snooze_options[ONE_HR].label, "1 hour");
    strcpy(snooze_options[SIX_HRS].label, "6 hours");
    strcpy(snooze_options[ONE_DAY].label, "1 day");
  }
  if(now) {
    if(DEBUG) {
      snooze_options[NEVER].time = now + 0;
      snooze_options[TEN_MIN].time = now + 5;
      snooze_options[ONE_HR].time = now + 15;
      snooze_options[SIX_HRS].time = now + 30;
      snooze_options[ONE_DAY].time = now + 60;
    } else {
      snooze_options[NEVER].time = now + 0;
      snooze_options[TEN_MIN].time = now + 600;
      snooze_options[ONE_HR].time = now + 3600;
      snooze_options[SIX_HRS].time = now + 21400;
      snooze_options[ONE_DAY].time = now + 86400;
    }
  }
  return snooze_options;
}

int next_snooze_opt_key(int current) {
  int next = current + 1;
  next = next >= SNOOZE_OPT_N ? 0 : next;
  return next;
}