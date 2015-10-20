#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <persistence.h>
#include <screen_list.h>
#include <screen_add.h>

static void init(void) {
  load_reminders();

  switch (launch_reason()) {
    case APP_LAUNCH_QUICK_LAUNCH:
      screen_add_show();
      break;
    case APP_LAUNCH_USER:
      //TODO show add screen if no reminder
    default:
      screen_list_show();
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
