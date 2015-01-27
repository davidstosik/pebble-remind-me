#include <pebble.h>
#include "log.h"

void log_method(char* method_name) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", method_name);
}

void log_reminder(Reminder reminder) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "REMINDER - { created_at: %d, schedule_at: %d }", (int)reminder.created_at, (int)reminder.schedule_at);
}

void log_all_saved_reminders() {
//   int count = persist_reminder_count();
//   int start = persist_reminder_start();
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "-- Starting log of all reminders (%d) (now = %d) --", count, (int)time(NULL));
//   for (int i=0; i < count; i++) {
//     persist_read_reminder(start + i);
// //     No need to log, call to persist_read_reminder above will log.
// //     log_reminder(reminder);
//   }
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "-- End of log of all reminders (%d) --", count);
}