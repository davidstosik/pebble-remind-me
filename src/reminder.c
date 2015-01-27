#include <pebble.h>
#include "constants.h"
#include "reminder.h"
#include "snooze_option.h"

Reminder reminders[MAX_REMINDERS];
int reminders_count;
Reminder reminder;

int sizeof_reminder() {
  return sizeof(struct ReminderStruct);
}

static int compare_by_schedule(const void * a, const void * b) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "compare_by_schedule");
  int f = (*((Reminder**)a))->schedule;
  int s = (*((Reminder**)b))->schedule;

  if (f == s) {
    return 0;
  } else if(f == 0) {
    return 1;
  } else if(s == 0) {
    return -1;
  } else {
    return (f > s) - (f < s);
  }
}

static Reminder** get_future_reminders_by_schedule(time_t from) {
  if (!from) {
    from = time(NULL);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "get_reminders_by_schedule");
  int size = reminders_count * sizeof(Reminder*);
  Reminder** result = malloc(size);
  //memcpy(result, reminders, size);
  int count = 0;
  for(int i=0; i < reminders_count; i++) {
    if (reminders[i].schedule > from) {
      result[count] = &reminders[i];
      count++;
    }
  }
  
  qsort(result, count, sizeof(Reminder*), compare_by_schedule);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Exits  %s", "get_reminders_by_schedule");
  return result;
}

static Reminder* get_next_scheduled_reminder(time_t from) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "get_next_scheduled_reminder");
  if (!from) {
    from = time(NULL);
  }
  if (reminders_count > 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "reminders_count > 0");
    Reminder ** sorted = get_future_reminders_by_schedule(0);
    int i= 0;
    do {
      if ((*sorted)[i].schedule > from) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "found next!");
        return sorted[i];
      }
    } while (++i < reminders_count);
    free(sorted);
  }
  return NULL;
}

void schedule_next_reminder_wakeup() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "schedule_next_reminder_wakeup");
  int wakeup_id = persist_read_int(WAKEUP_ID_KEY);
  if (wakeup_id >= 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Cancels previous wakeup_id: %d", wakeup_id);
    wakeup_cancel(wakeup_id);
    wakeup_id = -1;
  }
  
  Reminder *next;
  if ((next = get_next_scheduled_reminder(0))) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying to call wakeup_schedule");
    wakeup_id = wakeup_schedule(next->schedule, next->created_at, true);
    //while (wakeup_id == E_RANGE || wakeup_id == E_INVALID_ARGUMENT) {
    while (wakeup_id < 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed, trying again");
      //FIXME need to save this in original reminder (or change sort to sort_linked_list).
      if (wakeup_id == E_RANGE || wakeup_id == E_INVALID_ARGUMENT) {
        next->schedule += 60;
      }
      wakeup_id = wakeup_schedule(next->schedule, next->created_at, true);
    }
    if (wakeup_id < 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "wakeup_schedule failed with error %d", wakeup_id);
    }
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Saving wakeup_id: %d", wakeup_id);
  persist_write_int(WAKEUP_ID_KEY, wakeup_id);
}

void push_reminder() {
  reminder.created_at = time(NULL);
  reminder.snooze_opt = snooze_opt_key;
  
  time_t now = time(NULL);
  time_t wakeup_time = get_snooze_options(now)[reminder.snooze_opt].time;
  if (wakeup_time > now) {
    reminder.schedule = wakeup_time;
  } else {
    reminder.schedule = 0;
  }

  //FIXME: need to ensure count doesn't go over limit.
  reminders[reminders_count] = reminder;
  reminders_count++;
}

void save_reminders() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "save_reminders");
  if (reminders_count > 0) {
    time_t reminder_times[MAX_REMINDERS];
    for(int i=0; i < reminders_count; i++) {
      reminder_times[i] = reminders[i].created_at;
      persist_write_data(reminder_times[i], &reminders[i], sizeof_reminder());
    }
    persist_write_data(REMINDERS_KEY, reminder_times, reminders_count * sizeof(reminder_times[0]));
  } else {
    persist_delete(REMINDERS_KEY);
  }
}

void load_reminders() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "load_reminders");
  if(persist_exists(REMINDERS_KEY)) {
    int size = persist_get_size(REMINDERS_KEY);

    reminders_count = size / sizeof(time_t);
    time_t reminder_times[reminders_count];
    persist_read_data(REMINDERS_KEY, reminder_times, size);
    for(int i=0; i < reminders_count; i++) {
      persist_read_data(reminder_times[i], &reminders[i], sizeof_reminder());
    }
  } else {
    reminders_count = 0;
  }
}