#include <pebble.h>
#include "constants.h"
#include "reminder.h"
#include "snooze_option.h"
#include "tools.h"

#if DEBUG
#include "debug.h"
#endif

size_t sizeof_reminder() {
  return sizeof(struct ReminderStruct);
}

Reminder* new_reminder() {
  Reminder* result = malloc(sizeof_reminder());
  result->snooze_opt = SNOOZE_DEFAULT;
  return result;
}

int compare_reminders(const void * a, const void * b) {
  int f = ((Reminder*)a)->schedule_at;
  int s = ((Reminder*)b)->schedule_at;

  if (f == s) {
    f = ((Reminder*)a)->created_at;
    s = ((Reminder*)b)->created_at;
    return (f > s) - (f < s);
  } else if(f == 0) {
    return 1;
  } else if(s == 0) {
    return -1;
  } else {
    return (f > s) - (f < s);
  }
}

static bool reminder_is_past(Reminder* reminder) {
  return reminder->schedule_at == 0;
}

static void reminder_wakeup_delay(int seconds, int position, Reminder* reminder_ptr) {
//   bool self_allocated = false;
//   if (!reminder_ptr) {
//     reminder_ptr = malloc(sizeof_reminder());
//     self_allocated = true;
//     persist_read_reminder(position, reminder_ptr);
//   }

//   reminder_ptr->schedule_at += seconds;

//   if (position < persist_reminder_count() - 1) {
//     Reminder* next_ptr = malloc(sizeof_reminder());
//     persist_read_reminder(position+1, next_ptr);
//     if (compare_reminders(reminder_ptr, next_ptr) > 0) {
//       reminder_wakeup_delay(seconds, position+1, next_ptr);
//     }
//     free(next_ptr);
//   }
//   if (self_allocated) {
//     free(reminder_ptr);
//   }
}

static int reminder_wakeup_cancel() {
  int wakeup_id = 0;
  if (persist_exists(WAKEUP_ID_KEY)) {
    wakeup_id = persist_read_int(WAKEUP_ID_KEY);
    wakeup_cancel(wakeup_id);
    persist_delete(WAKEUP_ID_KEY);
  }
  return wakeup_id;
}

int reminder_wakeup_reschedule() {
//   int result = S_SUCCESS;
//   if (persist_reminder_count() > 0) {
//     reminder_wakeup_cancel();
//     Reminder* first_ptr = malloc(sizeof_reminder());
//     persist_read_reminder(0, first_ptr);
    
//     // We suppose the system works, and first reminder's schedule_at is in the future or 0.
//     // TODO also check if current schedule is the right one (then do nothing).
//     if (first_ptr->schedule_at > 0) {
//       result = wakeup_schedule(first_ptr->schedule_at, first_ptr->created_at, true);
//       while(result == E_RANGE || result == E_INVALID_ARGUMENT) {
//         reminder_wakeup_delay(60, 0, first_ptr);
//         result = wakeup_schedule(first_ptr->schedule_at, first_ptr->created_at, true);
//       }
    
//       persist_write_int(WAKEUP_ID_KEY, result);
//     }
//     free(first_ptr);
//   }
//   return result;
  return 0;
}

void reminder_insert(Reminder reminder, Reminder* *reminders_ptr, int *qty_ptr) {
  int start_time = full_time_ms();
  
  if (*reminders_ptr) {
    Reminder * tmp = realloc(*reminders_ptr, (1+*qty_ptr)*sizeof_reminder());
    if(tmp != NULL) *reminders_ptr = tmp;
  } else {
    *reminders_ptr = malloc((1+*qty_ptr)*sizeof_reminder());
  }
  (*reminders_ptr)[*qty_ptr] = reminder;
  *qty_ptr = *qty_ptr + 1;
  qsort(*reminders_ptr, *qty_ptr, sizeof_reminder(), compare_reminders);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG,
    "Inserted a reminder in a %d-element array, then sorted the result in %dms.",
    *qty_ptr - 1,
    (int)(full_time_ms() - start_time)
  );
}

void reminder_compute_schedule_at(Reminder* reminder_ptr) {
  reminder_ptr->schedule_at = snooze_time(reminder_ptr->snooze_opt, reminder_ptr->created_at);
}
