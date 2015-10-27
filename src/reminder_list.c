#include <pebble.h>
#include <reminder.h>
#include <reminder_list.h>

struct ReminderList* ReminderList_create() {
  struct ReminderList* result = malloc(sizeof(struct ReminderList));
  result->head = NULL;
  result->size = 0;
  return result;
}

int ReminderList_size(struct ReminderList* list) {
  return list->size;
}

static void _ReminderNode_unshift(struct ReminderNode** node, struct Reminder reminder) {
  struct ReminderNode* new_node = malloc(sizeof(struct ReminderNode));
  new_node->reminder = reminder;
  new_node->next = *node;
  *node = new_node;
}

void ReminderList_unshift(struct ReminderList* list, struct Reminder reminder) {
  _ReminderNode_unshift(&(list->head), reminder);
  list->size++;
}

static void _ReminderNode_shift(struct ReminderNode** node, struct Reminder* result) {
  if (node == NULL) {
    result = NULL;
  }
  else {
    struct ReminderNode* first_node = *node;
    *result = first_node->reminder;
    *node = first_node->next;
    free(first_node);
  }
}

void ReminderList_shift(struct ReminderList* list, struct Reminder* result) {
  _ReminderNode_shift(&(list->head), result);
  if (result) {
    list->size--;
  }
}

static void _ReminderNode_push(struct ReminderNode** node, struct Reminder reminder) {
  if (*node == NULL) {
    *node = malloc(sizeof(struct ReminderList));
    (*node)->reminder = reminder;
    (*node)->next = NULL;
  }
  else {
    _ReminderNode_push(&((*node)->next), reminder);
  }
}

void ReminderList_push(struct ReminderList* list, struct Reminder reminder) {
  _ReminderNode_push(&(list->head), reminder);
  list->size++;
}

static void _ReminderNode_pop(struct ReminderNode** node, struct Reminder* result) {
  if (*node == NULL) {
    result = NULL;
  }
  else if ((*node)->next == NULL) {
    *result = (*node)->reminder;
    free(*node);
    *node = NULL;
  }
  else {
    _ReminderNode_pop(&((*node)->next), result);
  }
}

void ReminderList_pop(struct ReminderList* list, struct Reminder* result) {
  _ReminderNode_pop(&(list->head), result);
  if (result) {
    list->size--;
  }
}

static void _ReminderNode_insert_sorted(struct ReminderNode** node, struct Reminder reminder) {
  if (*node == NULL|| Reminder_compare(reminder, (*node)->reminder) <= 0) {
    _ReminderNode_unshift(node, reminder);
  }
  else {
    _ReminderNode_insert_sorted(&((*node)->next), reminder);
  }
}

void ReminderList_insert_sorted(struct ReminderList* list, struct Reminder reminder) {
  _ReminderNode_insert_sorted(&(list->head), reminder);
  list->size++;
}

static struct ReminderNode** _ReminderNode_get_reminder_node_at(struct ReminderNode** node, int index) {
  if (*node == NULL || index < 0) {
    return NULL;
  }
  else if (index == 0) {
    return node;
  }
  else {
    return _ReminderNode_get_reminder_node_at(&((*node)->next), index - 1);
  }
}

void ReminderList_get_reminder_at(struct ReminderList* list, int index, struct Reminder* result) {
  struct ReminderNode** picked = _ReminderNode_get_reminder_node_at(&(list->head), index);
  if (picked) {
    *result = (*picked)->reminder;
  }
}

static void _ReminderNode_delete_at(struct ReminderNode** node, int index, struct Reminder* deleted) {
  struct ReminderNode** picked = _ReminderNode_get_reminder_node_at(node, index);
  _ReminderNode_shift(picked, deleted);
}

void ReminderList_delete_at(struct ReminderList* list, int index, struct Reminder* deleted) {
  _ReminderNode_delete_at(&(list->head), index, deleted);
  if (deleted) {
    list->size--;
  }
}

static void _ReminderNode_free(struct ReminderNode* node) {
  if (node) {
    _ReminderNode_free(node->next);
    free(node);
  }
}

void ReminderList_free(struct ReminderList* list) {
  _ReminderNode_free(list->head);
  free(list);
}

static void _ReminderNode_log_names(struct ReminderNode* node, char * list_str, bool first) {
  if (node == NULL) { return; }

  if (first) {
    strcpy(list_str, "");
  } else {
    strcat(list_str, ", ");
  }
  strcat(list_str, node->reminder.message);
  _ReminderNode_log_names(node->next, list_str, false);
}

void ReminderList_log_names(struct ReminderList* list) {
  char * list_str = malloc(500);
  _ReminderNode_log_names(list->head, list_str, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Debugging list: [%s]", list_str);
  free(list_str);
}
