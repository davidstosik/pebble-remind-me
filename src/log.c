#include <pebble.h>
#include "log.h"
#include "snooze_option.h"

void log_function(char* method_name) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", method_name);
}

void log_reminder(Reminder reminder) {
  APP_LOG(
    APP_LOG_LEVEL_DEBUG,
    "REMINDER - { created_at: %d, schedule_at: %d, snooze_opt: %s }",
    (int)reminder.created_at, (int)reminder.schedule_at, snooze_label(reminder.snooze_opt)
  );
}

void log_all_saved_reminders() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "-- Starting log of all reminders (now = %d) --", (int)time(NULL));
  Reminder reminder;
  int position = 0;
  while(persist_read_reminder(position, &reminder) >= 0) {
    log_reminder(reminder);
    position++;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "-- End of log of all reminders (%d) --", position);
}

uint32_t full_time_ms() {
  time_t   sec = 0;
  uint16_t ms  = 0;
  time_ms(&sec, &ms);
  return 1000 * sec + ms;
}