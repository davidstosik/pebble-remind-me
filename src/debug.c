#include <pebble.h>
#include "debug.h"

#include "main.h"
#include "persist_reminders.h"
#include "snooze_option.h"

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

void debug_reminders(Reminder* reminders, int qty) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "== REMINDERS (%d) ==", qty);
  for(int i = 0; i < qty; i++) {
    debug_reminder(reminders[i]);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "== END OF REMINDERS ==");
}

void init_debug_tests() {
  time_t now = time(NULL);
  srand(now);
  
  Reminder * test_reminders;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of empty reminders: %d", sizeof(test_reminders));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of reminders at start: %d", sizeof(reminders));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of a Reminder: %d", sizeof_reminder());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of a SnoozeOption: %d", sizeof(SnoozeOption));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of a bool: %d", sizeof(bool));

 if (DEBUG && reminders_qty == 0) {
    int one_day = 24 * 3600;
    time_t three_days_ago = now - 3 * one_day;
    int six_days = 6 * one_day;
    
    int total = PERSIST_REMINDERS_MAX - rand()%50;
  
    uint32_t start = full_time_ms();

    if(reminders) free(reminders);
    reminders = malloc(total * sizeof_reminder());

    for (int i=0; i < total; i++) {
      reminders[i].created_at = three_days_ago + rand()%six_days;
      reminders[i].snooze_opt = rand()%SNOOZE_OPT_N;
      reminder_compute_schedule_at(&reminders[i]);
      reminders[i].done = reminders[i].schedule_at < now ? !(rand()%3) : false;
    }
    reminders_qty = total;

    qsort(reminders, reminders_qty, sizeof_reminder(), compare_reminders);
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Filled %d reminders in %dms", total, (int)(full_time_ms() - start));
    
    Reminder to_insert;
    to_insert.created_at = now;
    to_insert.snooze_opt = TEN_MIN;
    reminder_compute_schedule_at(&to_insert);
    to_insert.done = to_insert.schedule_at < now ? !(rand()%3) : false;

    reminder_insert(to_insert, &reminders, &reminders_qty);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG,
    "The position of the first 'never' scheduled reminder is %d",
    find_first_never_reminder(reminders, reminders_qty)
  );

  APP_LOG(APP_LOG_LEVEL_DEBUG,
    "The position of the next scheduled reminder is %d",
    find_next_reminder(reminders, reminders_qty, 0)
  );
}

void deinit_debug_tests() {
//   debug_all_saved_reminders();
}
