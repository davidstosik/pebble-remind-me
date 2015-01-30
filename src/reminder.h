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
 * @return the memory size of a Reminder, in bytes.
 */
size_t sizeof_reminder();

/**
 * Allocate a new Reminder and initializes its attributes.
 * @return a pointer to the new reminder.
 */
Reminder* new_reminder();

/**
 * Compare two reminders, in a way compatible with qsort().
 * Schedule_at attribute is used as comparison, where "never" comes last.
 * For two reminders scheduled at the same time, the created_at attribute
 * will be used.
 * @param a pointer to the first reminder to compare.
 * @param a pointer to the second reminder to compare.
 * @return
 *  - a negative number if the first reminders comes sooner than the second
 *  - a positive number if the second reminders comes later than the second
 *  - 0 if both reminders are expected at the same time, and were created at the same time.
 */
int compare_reminders(const void * a, const void * b);

/**
 * Find by dichotomy the position of the first reminder never scheduled
 * in a list of sorted reminders (never scheduled reminders are at the end).
 * @param reminders an array of reminders.
 * @param qty the number of reminders contained in the array.
 * @return the position of the searched reminder in the array,
 *   or -1 if not found.
 */
int find_first_never_reminder(Reminder* reminders, int qty);
int find_next_reminder(Reminder* reminders, int qty, time_t now);

void reminder_count_types(Reminder* reminders, int qty, int values[3]);

/**
 * Insert a reminder in an array of reminders. Will reallocate memory.
 * Reminders are sorted with qsort(), using compare_reminders() to compare.
 * @param reminder the reminder to insert.
 * @param reminders_ptr a pointer to the array of reminders, passed by reference.
 *   It will be updated on memory (re)allocation.
 * @param qty_ptr the number of reminders contained in the array, passed by reference.
 *   It will be incremented after insertion.
 * 
 */
void reminder_insert(Reminder reminder, Reminder* *reminders_ptr, int *qty_ptr);

/**
 * Compute a reminder's schedule_at, based on its creation date and snooze option.
 * @param reminder_ptr a pointer to the reminder.
 */
void reminder_compute_schedule_at(Reminder* reminder_ptr);


/**
 * Reschedule wakeup call for next reminder.
 */
int reminder_wakeup_reschedule(Reminder* reminders, int qty);
