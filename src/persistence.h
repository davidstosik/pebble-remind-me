#pragma once
#include <reminder.h>

int get_reminder_count();
void load_reminders();
void get_reminder_at(int index, struct Reminder* result);
void reminders_add_reminder(struct Reminder new_reminder);
void reminders_delete_reminder(int index);
void persist_reminders();
void free_reminders();
