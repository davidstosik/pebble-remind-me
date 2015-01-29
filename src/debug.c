#include <pebble.h>
#include "debug.h"
#include "snooze_option.h"
#include "persist_reminders.h"

void debug_function(char* method_name) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", method_name);
}

void debug_reminder(Reminder reminder) {
  APP_LOG(
    APP_LOG_LEVEL_DEBUG,
    "REMINDER - { created_at: %d, schedule_at: %d, snooze_opt: %s }",
    (int)reminder.created_at, (int)reminder.schedule_at, snooze_label(reminder.snooze_opt)
  );
}

void debug_all_saved_reminders() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "-- Starting log of all reminders (now = %d) --", (int)time(NULL));
  Reminder reminder;
  int position = 0;
  while(persist_read_reminder(position, &reminder) >= 0) {
    debug_reminder(reminder);
    position++;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "-- End of log of all reminders (%d) --", position);
}

static Reminder* debug_reminders;
void init_debug_tests() {
//   debug_all_saved_reminders();

  srand(time(NULL));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of a Reminder: %d", sizeof_reminder());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of a SnoozeOption: %d", sizeof(SnoozeOption));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of a bool: %d", sizeof(bool));

  time_t jan_1st = 1420070400;
  int thirty_days = 30 * 24 * 3600;
  int total = 300 + rand()%100;

  uint32_t start = full_time_ms();
  debug_reminders = malloc(total * sizeof_reminder());
  for (int i=0; i < total; i++) {
    debug_reminders[i].schedule_at = rand()%3 ? 0 : (jan_1st + (rand()*rand())%thirty_days);
    debug_reminders[i].created_at = jan_1st + ((rand()*rand())%thirty_days);
    debug_reminders[i].snooze_opt = rand()%SNOOZE_OPT_N;
    debug_reminders[i].done = rand()%3;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Filled %d reminders in %dms", total, (int)(full_time_ms() - start));
  start = full_time_ms();
  qsort(debug_reminders, total, sizeof_reminder(), compare_reminders);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sorted %d reminders in %dms", total, (int)(full_time_ms() - start));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Compare function was called %d times", get_calls());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "First reminder's created_at: %d", (int)debug_reminders[0].created_at);
  
  
  persist_reminders_save(debug_reminders, total);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Reading count from storage: %d", (int)persist_read_int(PERSIST_REMINDERS_COUNT_KEY));
}

void deinit_debug_tests() {
  persist_reminders_read(debug_reminders);
  free(debug_reminders);
  debug_all_saved_reminders();
}