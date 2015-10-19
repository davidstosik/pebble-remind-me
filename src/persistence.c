#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <reminder_list.h>

static struct ReminderList* reminders;

void load_reminders() {
  static bool loaded = false;
  if (loaded) { return; }

  int reminder_count = 0;
  if (persist_exists(PERSIST_REMINDERS_COUNT_KEY)) {
    reminder_count = persist_read_int(PERSIST_REMINDERS_COUNT_KEY);
  }

  reminders = ReminderList_create();

  // FIXME batch reading/writing to maximise slot usage?
  for (int i = reminder_count - 1; i >= 0; i--) {
    struct Reminder reminder;
    persist_read_data(
      PERSIST_REMINDERS_START_KEY + i,
      &reminder,
      sizeof(reminder)
    );
    ReminderList_unshift(reminders, reminder);
  }
  loaded = true;
}

int get_reminder_count() {
  return ReminderList_size(reminders);
}

void get_reminder_at(int index, struct Reminder* result) {
  ReminderList_get_reminder_at(reminders, index, result);
}


void reminders_add_reminder(struct Reminder new_reminder) {
  ReminderList_insert_sorted(reminders, new_reminder);
}

void reminders_delete_reminder(int index) {
  struct Reminder deleted;
  ReminderList_delete_at(reminders, index, &deleted);
}

void persist_reminders() {
  struct ReminderNode * pointer = reminders->head;
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
