#pragma once
#include <reminder.h>

int get_reminder_count();
struct Reminder** load_reminders();
void free_reminders();
