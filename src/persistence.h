#pragma once
#include <reminder.h>

int get_reminder_count();
void load_reminders();
struct Reminder** get_reminders();
void reminders_add_reminder(struct Reminder * new_reminder);
void reminders_delete_reminder(int index);
void persist_reminders();
void free_reminders();
