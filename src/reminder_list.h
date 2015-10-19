#pragma once
#include <pebble.h>
#include <reminder.h>

struct ReminderNode {
  struct Reminder reminder;
  struct ReminderNode* next;
};
struct ReminderList {
  struct ReminderNode* head;
  int size;
};

struct ReminderList* ReminderList_create();
int ReminderList_size(struct ReminderList* list);
void ReminderList_unshift(struct ReminderList* list, struct Reminder reminder);
void ReminderList_shift(struct ReminderList* list, struct Reminder* result);
void ReminderList_push(struct ReminderList* list, struct Reminder reminder);
void ReminderList_pop(struct ReminderList* list, struct Reminder* result);
void ReminderList_insert_sorted(struct ReminderList* list, struct Reminder reminder);
void ReminderList_get_reminder_at(struct ReminderList* list, int index, struct Reminder* result);
void ReminderList_delete_at(struct ReminderList* list, int index, struct Reminder* deleted);
void ReminderList_free(struct ReminderList* list);
void ReminderList_log_names(struct ReminderList* list);
