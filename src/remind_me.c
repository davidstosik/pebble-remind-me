#include <pebble.h>
#include "notif.h"
#include "main.h"
#include "reminder.h"
#include "log.h"

static void wakeup_handler(WakeupId id, int32_t reason) {
  show_notif(id, reason);
}

static void init(void) {
  WakeupId id = 0;
  int32_t reason = 0;
  if (wakeup_get_launch_event(&id, &reason)) {
    // load_triggered_reminder
    wakeup_handler(id, reason);
  } else {
    show_main();
  }

  wakeup_service_subscribe(wakeup_handler);
}

static void deinit(void) {
  int wakeup_id = reminder_wakeup_reschedule();
  DEBUG ? APP_LOG(APP_LOG_LEVEL_DEBUG, "rescheduled wakeup with id %d", wakeup_id) : NULL;
  DEBUG ? log_all_saved_reminders() : NULL;
}

int main(void) {
  init();
  DEBUG ? log_all_saved_reminders() : NULL;
  app_event_loop();
  deinit();
}