#include <pebble.h>
#include "snooze_option.h"

static SnoozeOption options[SNOOZE_OPT_N];

SnoozeOption* snooze_options() {
  static bool set = false;
  if(!set) {
    for (int i = 0; i < SNOOZE_OPT_N - 1; i++) {
      options[i] = NEVER + i;
    }
    set = true;
  }
  return options;
}

char * snooze_label(SnoozeOption option) {
  switch(option) {
    case NEVER:   return "Never"   ;
    case TEN_MIN: return "10 min." ;
    case ONE_HR:  return "1 hour"  ;
    case SIX_HRS: return "6 hours" ;
    case ONE_DAY: return "1 day"   ;
    default:      return "Unknown" ;
  }
}

time_t snooze_time(SnoozeOption option, time_t now) {
  switch(option) {
    case NEVER:   return 0;
    case TEN_MIN: return now + (DEBUG ? 5  : 600   );
    case ONE_HR:  return now + (DEBUG ? 15 : 3600  );
    case SIX_HRS: return now + (DEBUG ? 30 : 21400 );
    case ONE_DAY: return now + (DEBUG ? 60 : 86400 );
    default:      return 0;
  }
}

SnoozeOption next_snooze_opt_key(SnoozeOption current) {
  SnoozeOption next = current + 1;
  next = next >= SNOOZE_OPT_N ? 0 : next;
  return next;
}