#include <pebble.h>
#include "constants.h"
#include "reminder.h"
#include "snooze_option.h"
#include "log.h"

// Reminder reminders[MAX_REMINDERS];
// int reminders_count;
// Reminder reminder;

int sizeof_reminder() {
  return sizeof(struct ReminderStruct);
}

Reminder* new_reminder() {
  Reminder* result = malloc(sizeof_reminder());
  result->snooze_opt = SNOOZE_DEFAULT;
  return result;
}

// static int compare_by_schedule(const void * a, const void * b) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "compare_by_schedule");
//   int f = (*((Reminder**)a))->schedule_at;
//   int s = (*((Reminder**)b))->schedule_at;

//   if (f == s) {
//     return 0;
//   } else if(f == 0) {
//     return 1;
//   } else if(s == 0) {
//     return -1;
//   } else {
//     return (f > s) - (f < s);
//   }
// }

// static Reminder** get_future_reminders_by_schedule(time_t from) {
//   if (!from) {
//     from = time(NULL);
//   }
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "get_reminders_by_schedule");
//   int size = reminders_count * sizeof(Reminder*);
//   Reminder** result = malloc(size);
//   //memcpy(result, reminders, size);
//   int count = 0;
//   for(int i=0; i < reminders_count; i++) {
//     if (reminders[i].schedule_at > from) {
//       result[count] = &reminders[i];
//       count++;
//     }
//   }
  
//   qsort(result, count, sizeof(Reminder*), compare_by_schedule);
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Exits  %s", "get_reminders_by_schedule");
//   return result;
// }

// static Reminder* get_next_scheduled_reminder(time_t from) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "get_next_scheduled_reminder");
//   if (!from) {
//     from = time(NULL);
//   }
//   if (reminders_count > 0) {
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "reminders_count > 0");
//     Reminder ** sorted = get_future_reminders_by_schedule(0);
//     int i= 0;
//     do {
//       if ((*sorted)[i].schedule_at > from) {
//         APP_LOG(APP_LOG_LEVEL_DEBUG, "found next!");
//         return sorted[i];
//       }
//     } while (++i < reminders_count);
//     free(sorted);
//   }
//   return NULL;
// }

// void schedule_next_reminder_wakeup() {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "schedule_next_reminder_wakeup");
//   int wakeup_id = persist_read_int(WAKEUP_ID_KEY);
//   if (wakeup_id >= 0) {
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Cancels previous wakeup_id: %d", wakeup_id);
//     wakeup_cancel(wakeup_id);
//     wakeup_id = -1;
//   }
  
//   Reminder *next;
//   if ((next = get_next_scheduled_reminder(0))) {
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying to call wakeup_schedule");
//     wakeup_id = wakeup_schedule(next->schedule_at, next->created_at, true);
//     //while (wakeup_id == E_RANGE || wakeup_id == E_INVALID_ARGUMENT) {
//     while (wakeup_id < 0) {
//       APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed, trying again");
//       //FIXME need to save this in original reminder (or change sort to sort_linked_list).
//       if (wakeup_id == E_RANGE || wakeup_id == E_INVALID_ARGUMENT) {
//         next->schedule_at += 60;
//       }
//       wakeup_id = wakeup_schedule(next->schedule_at, next->created_at, true);
//     }
//     if (wakeup_id < 0) {
//       APP_LOG(APP_LOG_LEVEL_DEBUG, "wakeup_schedule failed with error %d", wakeup_id);
//     }
//   }
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Saving wakeup_id: %d", wakeup_id);
//   persist_write_int(WAKEUP_ID_KEY, wakeup_id);
// }

Reminder persist_read_reminder(int address) {
 // log_method("persist_read_reminder");
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "address: %d", address);
  Reminder result;
  persist_read_data(address, &result, sizeof_reminder());
 // log_reminder(result);
  return result;
}

static void persist_write_reminder(int address, Reminder reminder) {
 // log_method("persist_write_reminder");
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "address: %d", address);
 // log_reminder(reminder);
  persist_write_data(address, &reminder, sizeof_reminder());
}

int persist_reminder_count() {
 // log_method("persist_reminder_count");
  int result = 0;
  if (persist_exists(REMINDERS_COUNT_KEY)) {
    result = persist_read_int(REMINDERS_COUNT_KEY);
  }
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "count: %d", result);
  return result;
}

int persist_reminder_start() {
 // log_method("persist_reminder_start");
  int result = REMINDERS_DEFAULT_START_KEY;
  if (persist_exists(REMINDERS_START_KEY)) {
    result = persist_read_int(REMINDERS_START_KEY);
  }
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "address: %d", result);
  return result;
}

void push_reminder(Reminder* reminder) {
  time_t sec = 0;
  uint16_t ms = 0;
  time_ms(&sec, &ms);
  uint32_t f_start = 1000 * sec + ms;

 // log_method("push_reminder");
  
  reminder->created_at = time(NULL);
  
  time_t now = time(NULL);
  time_t wakeup_time = get_snooze_options(now)[reminder->snooze_opt].time;
  if (wakeup_time > now) {
    reminder->schedule_at = wakeup_time;
  } else {
    reminder->schedule_at = 0;
  }
  
 // log_reminder(*reminder);
  
  int start = persist_reminder_start();
  int position = persist_reminder_count();
  bool found_position = reminder->schedule_at == 0;
  while(!found_position && position > 0) {
   // APP_LOG(APP_LOG_LEVEL_DEBUG, "Position not found yet, current = %d", position);
    Reminder previous = persist_read_reminder(start + position - 1);
   // log_reminder(previous);
    // TODO move comparison code to external function.
    if (previous.schedule_at == 0 || previous.schedule_at > reminder->schedule_at) {
      persist_write_reminder(start + position, previous);
      position--;
    } else {
     // APP_LOG(APP_LOG_LEVEL_DEBUG, "Found position %d", position);
      found_position = true;
    }
  }
  
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "Saving to position %d", position);
  persist_write_reminder(start + position, *reminder);
  persist_write_int(REMINDERS_COUNT_KEY, persist_reminder_count() + 1);
  //FIXME: need to ensure count doesn't go over some limit.
  
  time_ms(&sec, &ms);
  uint32_t f_end = 1000 * sec + ms;
 APP_LOG(APP_LOG_LEVEL_DEBUG, "push_reminder() ending. Time: %dms", (int)(f_end - f_start));
}

// void save_reminders() {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "save_reminders");
//   if (reminders_count > 0) {
//     time_t reminder_times[MAX_REMINDERS];
//     for(int i=0; i < reminders_count; i++) {
//       reminder_times[i] = reminders[i].created_at;
//       persist_write_data(reminder_times[i], &reminders[i], sizeof_reminder());
//     }
//     persist_write_data(REMINDERS_KEY, reminder_times, reminders_count * sizeof(reminder_times[0]));
//   } else {
//     persist_delete(REMINDERS_KEY);
//   }
// }

// void load_reminders() {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "load_reminders");
//   if(persist_exists(REMINDERS_KEY)) {
//     int size = persist_get_size(REMINDERS_KEY);

//     reminders_count = size / sizeof(time_t);
//     time_t reminder_times[reminders_count];
//     persist_read_data(REMINDERS_KEY, reminder_times, size);
//     for(int i=0; i < reminders_count; i++) {
//       persist_read_data(reminder_times[i], &reminders[i], sizeof_reminder());
//     }
//   } else {
//     reminders_count = 0;
//   }
// }