#pragma once
#include "reminder.h"

int persist_reminders_get_size();
void persist_reminders_save(Reminder* reminders, int qty);
void persist_reminders_read(Reminder* reminders);