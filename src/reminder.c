#include <pebble.h>
#include <reminder.h>

struct Reminder Reminder_create(char * message) {
  struct Reminder reminder;

  reminder.created_at = time(NULL);
  reminder.remind_at = reminder.created_at;
  strncpy(reminder.message, message, REMINDER_MESSAGE_MAX_LENGTH);

  return reminder;
}

int Reminder_compare(struct Reminder a, struct Reminder b) {
  return a.remind_at - b.remind_at;
}
