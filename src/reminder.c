#include <pebble.h>
#include "constants.h"
#include "reminder.h"
#include "snooze_option.h"
#include "log.h"

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

int persist_reminder_count() {
  int result = 0;
  if (persist_exists(REMINDERS_COUNT_KEY)) {
    result = persist_read_int(REMINDERS_COUNT_KEY);
  }
  return result;
}

int persist_reminder_start() {
  int result = REMINDERS_DEFAULT_START_KEY;
  if (persist_exists(REMINDERS_START_KEY)) {
    result = persist_read_int(REMINDERS_START_KEY);
  }
  return result;
}

static void reminder_wakeup_delay(int seconds, int position, Reminder* reminder_ptr) {
  bool self_allocated = false;
  if (!reminder_ptr) {
    reminder_ptr = malloc(sizeof_reminder());
    self_allocated = true;
    persist_read_reminder(position, reminder_ptr);
  }

  reminder_ptr->schedule_at += seconds;

  if (position < persist_reminder_count() - 1) {
    Reminder* next_ptr = malloc(sizeof_reminder());
    persist_read_reminder(position+1, next_ptr);
    if (compare_reminders(reminder_ptr, next_ptr) > 0) {
      reminder_wakeup_delay(seconds, position+1, next_ptr);
    }
    free(next_ptr);
  }
  if (self_allocated) {
    free(reminder_ptr);
  }
}

int reminder_wakeup_reschedule() {
  int result = S_SUCCESS;
  if (persist_reminder_count() > 0) {

    if (persist_exists(WAKEUP_ID_KEY)) {
      wakeup_cancel(persist_read_int(WAKEUP_ID_KEY));
    }
    Reminder* first_ptr = malloc(sizeof_reminder());
    persist_read_reminder(0, first_ptr);
    
    // We suppose the system works, and first reminder's schedule_at is in the future or 0.
    // TODO also check if current schedule is the right one (then do nothing).
    if (first_ptr->schedule_at > 0) {
      result = wakeup_schedule(first_ptr->schedule_at, first_ptr->created_at, true);
      while(result == E_RANGE || result == E_INVALID_ARGUMENT) {
        reminder_wakeup_delay(60, 0, first_ptr);
        result = wakeup_schedule(first_ptr->schedule_at, first_ptr->created_at, true);
      }
    
      persist_write_int(WAKEUP_ID_KEY, result);
    }
    free(first_ptr);
  }
  return result;
}

int persist_read_reminder(int position, Reminder* buff) {
  if (position < 0 || position >= persist_reminder_count()) {
    return E_RANGE;
  } else {
    return persist_read_data(persist_reminder_start() + position, buff, sizeof_reminder());
  }
}

static int persist_write_reminder(int position, Reminder* data) {
  if (position < 0) {
    return E_RANGE;
  } else {
    return persist_write_data(persist_reminder_start() + position, data, sizeof_reminder());
  }
}

static int persist_delete_reminder(int position) {
  int count = persist_reminder_count();
  if (position < 0 || position >= count) {
    return E_RANGE;
  } else {
    status_t status = persist_delete(persist_reminder_start() + position);
    Reminder* buff = malloc(sizeof_reminder());
    for (int i=position; i < count-1; i++) {
      persist_read_reminder(i+1, buff);
      persist_write_reminder(i, buff);
    }
    free(buff);
    return status;
  }
}

int persist_pull_reminder(int position, Reminder* buff) {
  int result = E_UNKNOWN;
  if (position < 0 || position >= persist_reminder_count()) {
    result = E_RANGE;
  } else if (persist_read_reminder(position, buff) > 0) {
    persist_write_int(REMINDERS_COUNT_KEY, persist_reminder_count() - 1);
    result = persist_delete_reminder(position);
  }
  return result;
}

void persist_push_reminder(Reminder* reminder_ptr) {
  /** DEBUG **/ uint32_t f_start = full_time_ms();

  time_t now = time(NULL);
  reminder_ptr->created_at = now;
  time_t wakeup_time =  snooze_time(reminder_ptr->snooze_opt, now);

  if (wakeup_time > now) {
    reminder_ptr->schedule_at = wakeup_time;
  } else {
    reminder_ptr->schedule_at = 0;
  }

  // TODO: (improvement) start from beginning if right position is closer.
  int position = persist_reminder_count();
  bool found_position = reminder_ptr->schedule_at == 0;
  Reminder previous;
  while(!found_position && position > 0) {
    persist_read_reminder(position - 1, &previous);
    if (compare_reminders(&previous, reminder_ptr) > 0) {
      persist_write_reminder(position, &previous);
      position--;
    } else {
      found_position = true;
    }
  }

  //FIXME: need to ensure count doesn't go over some limit.
  persist_write_reminder(position, reminder_ptr);
  persist_write_int(REMINDERS_COUNT_KEY, persist_reminder_count() + 1);

  DEBUG ? APP_LOG(APP_LOG_LEVEL_DEBUG, "push_reminder() ending. Time: %dms", (int)(full_time_ms() - f_start)) : NULL;
}

void persist_destroy_all_reminders() {
  int start = persist_reminder_start();
  for (int i=0; i < persist_reminder_count(); i++) {
    persist_delete(start + i);
  }
  persist_write_int(REMINDERS_COUNT_KEY, 0);
}