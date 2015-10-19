#include <pebble.h>
#include <reminder.h>
#include <reminder_list.h>

struct ReminderList* ReminderList_create() {
  return NULL;
}

bool ReminderList_is_empty(struct ReminderList* list) {
  return list == NULL;
}

int ReminderList_size(struct ReminderList* list) {
  if (ReminderList_is_empty(list)) {
    return 0;
  }
  else {
    return 1 + ReminderList_size(list->next);
  }
}

void ReminderList_unshift(struct ReminderList** list, struct Reminder reminder) {
  struct ReminderList* new_node = malloc(sizeof(struct ReminderList));
  new_node->reminder = reminder;
  new_node->next = *list;
  *list = new_node;
}

void ReminderList_shift(struct ReminderList** list, struct Reminder* result) {
  if (ReminderList_is_empty(*list)) {
    result = NULL;
  }
  else {
    struct ReminderList* first_node = *list;
    *list = (*list)->next;
    *result = first_node->reminder;
    free(first_node);
  }
}

void ReminderList_push(struct ReminderList** list, struct Reminder reminder) {
  if (ReminderList_is_empty(*list)) {
    *list = malloc(sizeof(struct ReminderList));
    (*list)->reminder = reminder;
    (*list)->next = NULL;
  }
  else {
    ReminderList_push(&((*list)->next), reminder);
  }
}

void ReminderList_pop(struct ReminderList** list, struct Reminder* result) {
  if (ReminderList_is_empty(*list)) {
    result = NULL;
  }
  else if (ReminderList_is_empty((*list)->next)) {
    *result = (*list)->reminder;
    free(*list);
    *list = NULL;
  }
  else {
    ReminderList_pop(&((*list)->next), result);
  }
}

void ReminderList_insert_sorted(struct ReminderList** list, struct Reminder reminder) {
  if (ReminderList_is_empty(*list) || Reminder_compare(reminder, (*list)->reminder) <= 0) {
    ReminderList_unshift(list, reminder);
  }
  else {
    ReminderList_insert_sorted(&((*list)->next), reminder);
  }
}

void ReminderList_delete_at(struct ReminderList** list, int index, struct Reminder* deleted) {
  if (ReminderList_is_empty(*list) || index < 0) {
    deleted = NULL;
  }
  else if (index == 0) {
    ReminderList_shift(list, deleted);
  }
  else {
    ReminderList_delete_at(&((*list)->next), index - 1, deleted);
  }
}

void ReminderList_free(struct ReminderList* list) {
  if (!ReminderList_is_empty(list)) {
    ReminderList_free(list->next);
    free(list);
  }
}

void _ReminderList_log_names(struct ReminderList* list, char * list_str, bool first) {
  if (list == NULL) { return; }

  if (first) {
    strcpy(list_str, "");
  } else {
    strcat(list_str, ", ");
  }
  strcat(list_str, list->reminder.label);
  _ReminderList_log_names(list->next, list_str, false);
}

void ReminderList_log_names(struct ReminderList* list) {
  char * list_str = malloc(500);
  _ReminderList_log_names(list, list_str, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Debugging list: [%s]", list_str);
  free(list_str);
}
