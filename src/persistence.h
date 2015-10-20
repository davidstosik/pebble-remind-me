#pragma once
#include <reminder.h>

struct ReminderList* all_reminders;

void load_reminders();
void persist_reminders();
void free_reminders();
