#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <reminder_list.h>

static struct ReminderList *reminders;
static int reminder_count = -1;

void load_reminders() {
  static bool loaded = false;
  if (loaded) { return; }

  if (persist_exists(PERSIST_REMINDERS_COUNT_KEY)) {
    reminder_count = persist_read_int(PERSIST_REMINDERS_COUNT_KEY);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_read_int(%i) = %i", PERSIST_REMINDERS_COUNT_KEY, reminder_count);
  }
  else {
    reminder_count = 0;
  }

  reminders = ReminderList_create();

  // FIXME batch reading/writing to maximise slot usage?
  for (int i = 0; i < reminder_count; i++) {
    struct Reminder reminder;
    persist_read_data(
      PERSIST_REMINDERS_START_KEY + i,
      &reminder,
      sizeof(reminder)
    );
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_read_data(%i) = reminder", PERSIST_REMINDERS_START_KEY + i);
    //FIXME push is unoptimal as it scans the whole list on each item. Use unshift from end of memory.
    ReminderList_push(&reminders, reminder);
  }
  loaded = true;
}

static int refresh_reminder_count(bool refresh) {
  if (reminder_count == -1 || refresh) {
    reminder_count = ReminderList_size(reminders);
  }
  return reminder_count;
}

int get_reminder_count() {
  return refresh_reminder_count(false);
}

struct ReminderList* get_reminders() {
  return reminders;
}

struct Reminder* _get_reminder_at(int index, struct ReminderList * list) {
  if (list == NULL) {
    return NULL;
  }
  else if (index == 0) {
    return &(list->reminder);
  }
  else {
    return _get_reminder_at(index-1, list->next);
  }
}

struct Reminder* get_reminder_at(int index) {
  return _get_reminder_at(index, reminders);
}

// static void* iso_realloc(void* ptr, size_t size) {
//   if (ptr != NULL) {
//     return realloc(ptr, size);
//   } else {
//     return malloc(size);
//   }
// }

void reminders_add_reminder(struct Reminder new_reminder) {
  ReminderList_insert_sorted(&reminders, new_reminder);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "reminders_add_reminder() ran, new size: %i", ReminderList_size(reminders));
  reminder_count++;
}

void reminders_delete_reminder(int index) {
  struct Reminder deleted;
  ReminderList_delete_at(&reminders, index, &deleted);
  //FIXME should never go below 0
  reminder_count--;
}

void persist_reminders() {
  struct ReminderList * pointer = reminders;
  int mem_key = PERSIST_REMINDERS_START_KEY;
  while (pointer != NULL) {
    persist_write_data(
      mem_key,
      &(pointer->reminder),
      sizeof(struct Reminder)
    );
    pointer = pointer->next;
    mem_key++;
  }

  while (persist_exists(mem_key) && mem_key <= PERSIST_REMINDERS_END_KEY) {
    persist_delete(mem_key);
    mem_key++;
  }

  persist_write_int(PERSIST_REMINDERS_COUNT_KEY, get_reminder_count());
}

void free_reminders() {
  ReminderList_free(reminders);
}
