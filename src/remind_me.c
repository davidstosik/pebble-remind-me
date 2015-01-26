#include <pebble.h>

#include "constants.h"
#include "reminder.h"
#include "notif.h"
#include "main.h"

static Window *window;

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "init");
  load_reminders();
  
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    // load_triggered_reminder
    show_notif();
  } else {
    // FIXME maybe reminder time should be set only on exit.
    init_reminder();
    show_main();
  }
}

static void deinit(void) {
  // FIXME should this go somewhere else?
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}