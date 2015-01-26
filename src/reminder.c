#include <pebble.h>
#include "constants.h"
#include "reminder.h"

Reminder reminders[MAX_REMINDERS];
int reminders_count;
Reminder reminder;

void init_reminder(Reminder *o) {
  o->created_at = time(NULL);
}

int sizeof_reminder() {
  return sizeof(struct ReminderStruct);
}

void add_reminder(Reminder reminder) {
  reminder.wakeup_id = wakeup_schedule(reminder.created_at + 10, reminder.created_at, true);
  //FIXME: need to ensure count doesn't go over limit.
  reminders[reminders_count] = reminder;
  reminders_count++;
}

void save_reminders() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "save_reminders");
  if (reminders_count > 0) {
    time_t reminder_times[MAX_REMINDERS];
    for(int i=0; i < reminders_count; i++) {
      reminder_times[i] = reminders[i].created_at;
      persist_write_data(reminder_times[i], &reminders[i], sizeof_reminder());
    }
    persist_write_data(REMINDERS_KEY, reminder_times, reminders_count * sizeof(reminder_times[0]));
  } else {
    persist_delete(REMINDERS_KEY);
  }
}

void load_reminders() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "load_reminders");
  if(persist_exists(REMINDERS_KEY)) {
    int size = persist_get_size(REMINDERS_KEY);

    reminders_count = size / sizeof(time_t);
    time_t reminder_times[reminders_count];
    persist_read_data(REMINDERS_KEY, reminder_times, size);
    for(int i=0; i < reminders_count; i++) {
      persist_read_data(reminder_times[i], &reminders[i], sizeof_reminder());
    }
  } else {
    reminders_count = 0;
  }
}