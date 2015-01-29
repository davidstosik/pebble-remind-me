#include <pebble.h>
#include "main.h"
#include "notif.h"
#include "snooze_option.h"
#include "add.h"
#include "reminder.h"
#include "persist_reminders.h"
#include "tools.h"

#if DEBUG
#include "debug.h"
#endif

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
  if (reminders_qty > 0) {
    reminders = malloc(reminders_qty * sizeof_reminder());
    persist_reminders_read(reminders);
  }

  #if DEBUG
  init_debug_tests();
  #endif
}

static void deinit(void) {
  reminder_wakeup_reschedule();
  
  // TODO: add a way to know if reminders changed or not (no need to persist if nothing changed).
  persist_reminders_write(reminders, reminders_qty);
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Before free(reminders); addr= %d", (int)reminders);
  free(reminders);

  #if DEBUG
  deinit_debug_tests();
  #endif
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
