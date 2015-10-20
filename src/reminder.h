#pragma once
#include <pebble.h>

struct Reminder {
  time_t created_at;
  time_t remind_at;
  char * label;
};

struct Reminder Reminder_create(int remind_in);
int Reminder_compare(struct Reminder a, struct Reminder b);
