#pragma once
#include "reminder.h"

void persist_reminders_destroy_all();
int persist_reminders_get_size();
void persist_reminders_write(Reminder* reminders, int qty);
void persist_reminders_read(Reminder* reminders);