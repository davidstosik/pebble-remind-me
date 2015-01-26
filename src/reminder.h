#include <pebble.h>
#include "constants.h"

struct ReminderStruct {
  time_t created_at;
  WakeupId wakeup_id;
};

typedef struct ReminderStruct Reminder;

Reminder reminders[MAX_REMINDERS];
int reminders_count;
Reminder reminder;

void init_reminder(Reminder*);

void add_reminder(Reminder);

void save_reminders();
void load_reminders();