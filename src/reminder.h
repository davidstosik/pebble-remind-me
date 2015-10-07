#pragma once
#include <pebble.h>

struct Reminder {
  time_t created_at;
  time_t remind_at;
};

struct Reminder *Reminder_create(int remind_in);
int get_reminder_count();
