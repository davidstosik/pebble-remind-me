#include <pebble.h>
#include <constants.h>
#include <reminder.h>

static struct Reminder **reminders;
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

  reminders = malloc(sizeof(struct Reminder*) * reminder_count);

  // FIXME batch reading?
  for (int i = 0; i < reminder_count; i++) {
    reminders[i] = malloc(sizeof(struct Reminder));
    persist_read_data(
      PERSIST_REMINDERS_START_KEY + i,
      reminders[i],
      sizeof(struct Reminder)
    );
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_read_data(%i) = reminder", PERSIST_REMINDERS_START_KEY + i);
  }
  loaded = true;
}

int get_reminder_count() {
  return reminder_count;
}

struct Reminder** get_reminders() {
  return reminders;
}

static void* iso_realloc(void* ptr, size_t size) {
  if (ptr != NULL) {
    return realloc(ptr, size);
  } else {
    return malloc(size);
  }
}

void reminders_add_reminder(struct Reminder * new_reminder) {
  reminder_count += 1;
  reminders = iso_realloc(reminders, reminder_count * sizeof(struct Reminder));
  reminders[reminder_count - 1] = new_reminder;
}

void reminders_delete_reminder(int index) {
  reminder_count--;
}

void persist_reminders() {
  for (int i = 0; i < reminder_count; i++) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_write(%i, reminder)", PERSIST_REMINDERS_START_KEY + i);
    persist_write_data(
      PERSIST_REMINDERS_START_KEY + i,
      reminders[i],
      sizeof(struct Reminder)
    );
  }

  int i = PERSIST_REMINDERS_START_KEY + reminder_count;
  while (persist_exists(i) && i < PERSIST_REMINDERS_END_KEY) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_delete(%i)", i);
    persist_delete(PERSIST_REMINDERS_START_KEY + i);
    i++;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_write(%i, %i)", PERSIST_REMINDERS_COUNT_KEY, reminder_count);
  persist_write_int(PERSIST_REMINDERS_COUNT_KEY, reminder_count);
}

void free_reminders() {
  for (int i = 0; i < reminder_count; i++) {
    free(reminders[i]);
  }
  free(reminders);
}
