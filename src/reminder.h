#pragma once
#include <pebble.h>
#include "constants.h"

struct ReminderStruct {
  time_t created_at;
  time_t schedule_at;
  short snooze_opt;
};

typedef struct ReminderStruct Reminder;

// Reminder reminders[MAX_REMINDERS];
// int reminders_count;
// Reminder reminder;

Reminder* new_reminder();

Reminder persist_read_reminder(int);
int persist_reminder_count();
int persist_reminder_start();
void push_reminder(Reminder*);

// void save_reminders();
// void load_reminders();

// void schedule_next_reminder_wakeup();