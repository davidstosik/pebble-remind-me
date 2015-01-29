#include <pebble.h>
#include "notif.h"
#include "snooze_option.h"
#include "main.h"
#include "reminder.h"
#include "persist_reminders.h"
#include "tools.h"

#if DEBUG
#include "debug.h"
#endif
  
Reminder* reminders;
int reminders_qty;

static void wakeup_handler(WakeupId id, int32_t reason) {
  show_notif(id, reason);
}

static void init(void) {
  wakeup_service_subscribe(wakeup_handler);

  WakeupId id = 0;
  int32_t reason = 0;
  if (wakeup_get_launch_event(&id, &reason)) {
    // load_triggered_reminder
    wakeup_handler(id, reason);
  } else {
    show_main();
  }

  reminders_qty = persist_reminders_get_size();
  reminders = malloc(reminders_qty * sizeof_reminder());
  persist_reminders_read(reminders);

  #if DEBUG
  init_debug_tests();
  #endif
}

static void deinit(void) {
  reminder_wakeup_reschedule();

  #if DEBUG
  deinit_debug_tests();
  #endif
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}