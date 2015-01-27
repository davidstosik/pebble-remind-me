#include <pebble.h>

#include "constants.h"
#include "reminder.h"
#include "notif.h"
#include "main.h"

static void wakeup_handler(WakeupId id, int32_t reason) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "wakeup_handler");
  schedule_next_reminder_wakeup();
  show_notif(id, reason);
}

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "init");
//   load_reminders();
  
  //if (launch_reason() == APP_LAUNCH_WAKEUP) {
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
//   schedule_next_reminder_wakeup();
//   save_reminders();
  push_reminder();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}