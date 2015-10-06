#include <pebble.h>
#include <reminder.h>

struct Reminder *Reminder_create(int remind_in) {
  struct Reminder *reminder = malloc(sizeof(struct Reminder));

  reminder->created_at = time(NULL);

  if (remind_in == -1) {
    remind_in = 10;
  }
  reminder->remind_at = reminder->created_at + remind_in;

  return reminder;
}
