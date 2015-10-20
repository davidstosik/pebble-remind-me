#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <reminder_list.h>
#include <persistence.h>

void load_reminders() {
  static bool loaded = false;
  if (loaded) { return; }

  int reminder_count = 0;
  if (persist_exists(PERSIST_REMINDERS_COUNT_KEY)) {
    reminder_count = persist_read_int(PERSIST_REMINDERS_COUNT_KEY);
  }

  all_reminders = ReminderList_create();

  // FIXME batch reading/writing to maximise slot usage?
  for (int i = reminder_count - 1; i >= 0; i--) {
    struct Reminder reminder;
    persist_read_data(
      PERSIST_REMINDERS_START_KEY + i,
      &reminder,
      sizeof(reminder)
    );
    ReminderList_unshift(all_reminders, reminder);
  }
  loaded = true;
}

void persist_reminders() {
  struct ReminderNode * pointer = all_reminders->head;
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

  persist_write_int(PERSIST_REMINDERS_COUNT_KEY, ReminderList_size(all_reminders));
}

void free_reminders() {
  ReminderList_free(all_reminders);
}
