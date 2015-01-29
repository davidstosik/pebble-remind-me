#pragma once
#include <pebble.h>
#include "snooze_option.h"

/**
 * A reminder saved by the user.
 */
#pragma pack(2)
struct ReminderStruct {
  //! The time when this reminder was created by the user. Acts as unique id,
  //! on the assumption that two reminders cannot be created at the same time.
  time_t created_at;
  //! The time when the application is supposed to notify the user.
  time_t schedule_at;
  //! A key that describes which timing option the user chose (@see snooze_option.h).
  SnoozeOption snooze_opt;
  //! A boolean state that indicates if the reminder is done.
  bool done;
};
typedef struct ReminderStruct Reminder;

/**
 * Return the memory size of a Reminder.
 * @return the memory size of a Reminder, in bytes.
 */
size_t sizeof_reminder();

/**
 * Allocate and initialize a new Reminder object.
 * @return a pointer to the created Reminder.
 */
Reminder* new_reminder();


int compare_reminders(const void * a, const void * b);
int get_calls();

/**
 * Read a previously persisted Reminder from storage.
 * @param position: the position of the reminder to read (starting from 0).
 * @param buff: the pointer to a buffer to be written to.
 * @return the number of bytes written into the buffer or a negative value if an error occurred:
 *   - E_DOES_NOT_EXIST if there is no field matching the given key
 *   - E_RANGE if the passed position is invalid
 */
int persist_read_reminder(int position, Reminder* buff);

int persist_write_reminder(int position, Reminder* data);

/**
 * Get the quantity of Reminders persisted in storage.
 * @return the quantity of Reminders currently persisted in storage.
 */
int persist_reminder_count();

int persist_reminder_get_first_past_position();
int persist_reminder_get_first_future_position();
int persist_reminder_count_future();
int persist_reminder_count_past();

/**
 * Persist a reminder in storage. The reminder will be placed so that the list of
 * reminders is sorted by schedule_at ascending.
 * (schedule_at==0, ie. "Never" reminders are put at the end of the list.)
 * @param reminder_ptr: a pointer to the reminder to write in storage.
 */
void persist_push_reminder(Reminder* reminder_ptr);

/**
 * Pull a reminder out of storage.
 * Reminder is read, deleted, and all following reminders are shifted.
 * @param position: the position of the reminder to pull (starting from 0).
 * @param buff: the pointer to a buffer to be written to.
 * @return the number of bytes written into the buffer or a negative value if an error occurred:
 *   - E_DOES_NOT_EXIST if there is no field matching the given key
 *   - E_RANGE if the passed position is invalid
 *   - E_UNKNOWN if anything else went wrong
 */
int persist_pull_reminder(int position, Reminder* buff);

/**
 * Reschedule wakeup call for next reminder.
 */
int reminder_wakeup_reschedule();

/**
 * Destroy all reminders from storage.
 */
void persist_destroy_all_reminders();