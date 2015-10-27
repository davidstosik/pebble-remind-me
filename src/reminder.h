#pragma once
#include <pebble.h>

#define REMINDER_MESSAGE_MAX_LENGTH 192

struct Reminder {
  time_t created_at;
  time_t remind_at;
  char message[REMINDER_MESSAGE_MAX_LENGTH];
};

struct Reminder Reminder_create(char * message);
int Reminder_compare(struct Reminder a, struct Reminder b);
