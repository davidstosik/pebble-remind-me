#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <persistence.h>
#include <screen_list.h>

static void init(void) {
  load_reminders();

  switch (launch_reason()) {
    case APP_LAUNCH_QUICK_LAUNCH:
      screen_list_show(true);
      break;
    case APP_LAUNCH_USER:
      //TODO show add screen if no reminder
    default:
      screen_list_show(false);
      break;
  }
}

static void deinit(void) {
  persist_reminders();
  free_reminders();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
