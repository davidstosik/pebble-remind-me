#include <pebble.h>
#include "reminder.h"

#include "constants.h"
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

typedef bool(* ReminderCondition)(Reminder reminder, void *callback_context);

static bool _reminder_is_never_scheduled_condition(Reminder reminder, void *callback_context) {
  return reminder.schedule_at == 0;
}

static bool _reminder_is_scheduled_in_the_future_condition(Reminder reminder, void *callback_context) {
  return reminder.schedule_at > *(time_t*)callback_context;
}

static bool _reminder_is_placed_after(Reminder reminder, void *callback_context) {
  return compare_reminders(&reminder, (Reminder*)callback_context) >= 0;
}

static int _dichotomy_find_reminder(Reminder* reminders, int qty, ReminderCondition condition, void *callback_context) {
  int offset = 0;

  recurse:
  if (qty == 0) {
    return E_DOES_NOT_EXIST;
  } else if (qty == 1) {
    if (condition(reminders[0], callback_context)) {
      return offset;
    } else {
      // Should not happen.
      return E_DOES_NOT_EXIST;
    }
  } else {
    int pivot = (int)((qty-1)/2) + 1;
    if (condition(reminders[pivot-1], callback_context)) {
      qty = pivot;
    } else {
      reminders = &reminders[pivot];
      qty -= pivot;
      offset += pivot;
    }
    // Iterate rather than recurse to save stack space. @see qsort()
    // for inspiration: http://www.jbox.dk/sanos/source/lib/qsort.c.html
    goto recurse;
  }
}

int find_first_never_reminder(Reminder* reminders, int qty) {
  return _dichotomy_find_reminder(reminders, qty, _reminder_is_never_scheduled_condition, NULL);
}

int find_next_reminder(Reminder* reminders, int qty, time_t now) {
  if (!now) now = time(NULL);
  
  int first_never = find_first_never_reminder(reminders, qty);
  if (first_never != E_DOES_NOT_EXIST) qty = first_never;

  return _dichotomy_find_reminder(reminders, qty, _reminder_is_scheduled_in_the_future_condition, &now);
}

/* TODO
get number of past/future/never reminders:
use find_next_reminder and find_first_never_reminder
to avoid recounting all the time, keeps values static and try
to mark the reminders array as "clean"
Maybe a struct that contains the array and the values?
*/

void reminder_count_types(Reminder* reminders, int qty, int values[3]) {
  int next, first_never;
  next = find_next_reminder(reminders, qty, time(NULL));
  first_never = find_first_never_reminder(reminders, qty);
  first_never = first_never == E_DOES_NOT_EXIST ? qty : first_never;
  next = next == E_DOES_NOT_EXIST ? first_never : next;
  values[0] = next;                    // Past
  values[1] = first_never - next;      // Future
  values[2] = qty - first_never;       // Never schedule
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

int reminder_wakeup_reschedule(Reminder* reminders, int qty) {
  reminder_wakeup_cancel();
  if (qty == 0) return S_SUCCESS;
  
  reminder_wakeup_cancel();
  int next = find_next_reminder(reminders, qty, time(NULL));
  if (next == E_DOES_NOT_EXIST) return S_SUCCESS;

  int result = wakeup_schedule(reminders[next].schedule_at, reminders[next].created_at, true);
// FIXME: might need to reschedule if another app took the slot.
//   while(result == E_RANGE || result == E_INVALID_ARGUMENT) {
//     reminder_wakeup_delay(60, 0, first_ptr);
//     result = wakeup_schedule(first_ptr->schedule_at, first_ptr->created_at, true);
//   }

  persist_write_int(WAKEUP_ID_KEY, result);
  return result;
}

void reminder_insert(Reminder reminder, Reminder* *reminders_ptr, int *qty_ptr) {
  int start_time = full_time_ms();
  
//   if (*reminders_ptr) {
//     Reminder * tmp = realloc(*reminders_ptr, (1+*qty_ptr)*sizeof_reminder());
//     if(tmp != NULL) *reminders_ptr = tmp;
//   } else {
//     *reminders_ptr = malloc((1+*qty_ptr)*sizeof_reminder());
//   }
  *reminders_ptr = iso_realloc(*reminders_ptr, (1+*qty_ptr)*sizeof_reminder());
  (*reminders_ptr)[*qty_ptr] = reminder;
  
  // Simplified insert sort algorithm (whole array is sorted but last item) is faster
  // (or as fast in worst case) as quick sort.
  // qsort(*reminders_ptr, *qty_ptr + 1, sizeof_reminder(), compare_reminders);
  int index = *qty_ptr;
  while (compare_reminders(&(*reminders_ptr)[index-1], &(*reminders_ptr)[index]) > 0 && index > 0) {
    Reminder tmp = (*reminders_ptr)[index-1];
    (*reminders_ptr)[index-1] = (*reminders_ptr)[index];
    (*reminders_ptr)[index] = tmp;
    index--;
  }
  
  *qty_ptr = *qty_ptr + 1;
  
  APP_LOG(APP_LOG_LEVEL_DEBUG,
    "Inserted a reminder in a %d-element array, then sorted (insert) in %dms.",
    *qty_ptr - 1,
    (int)(full_time_ms() - start_time)
  );
}

void reminder_compute_schedule_at(Reminder* reminder_ptr) {
  reminder_ptr->schedule_at = snooze_time(reminder_ptr->snooze_opt, reminder_ptr->created_at);
}
