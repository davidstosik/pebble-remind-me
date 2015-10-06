#include <pebble.h>
#include <reminder.h>

static struct Reminder **reminders;
static int reminder_count = -1;

int get_reminder_count() {
  if (reminder_count == -1) {
    reminder_count = 5;
  }
  return reminder_count;
}

struct Reminder** load_reminders() {
  static bool loaded = false;

  if (!loaded) {
    reminders = malloc(sizeof(struct Reminder*) * get_reminder_count());
    reminders[0] = Reminder_create(30);
    reminders[1] = Reminder_create(600);
    reminders[2] = Reminder_create(1800);
    reminders[3] = Reminder_create(3600);
    reminders[4] = Reminder_create(86400);
    loaded = true;
  }

  return reminders;
}

void free_reminders() {
  for (int i = 0; i < get_reminder_count(); i++) {
    free(reminders[i]);
  }
  free(reminders);
}
