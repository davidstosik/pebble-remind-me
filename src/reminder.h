#include <pebble.h>
#include "constants.h"

struct ReminderStruct {
  time_t created_at;
  time_t schedule;
  short snooze_opt;
};

typedef struct ReminderStruct Reminder;

Reminder reminders[MAX_REMINDERS];
int reminders_count;
Reminder reminder;

void push_reminder();

void save_reminders();
void load_reminders();

void schedule_next_reminder_wakeup();