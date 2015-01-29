#pragma once
#include "reminder.h"

void debug_function(char*);
void debug_reminder(Reminder);
void debug_reminders(Reminder* reminders, int qty);
void debug_all_saved_reminders();
uint32_t full_time_ms();

void init_debug_tests();
void deinit_debug_tests();
