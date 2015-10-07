#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <persistence.h>
#include <screen_list.h>
#include <screen_add.h>

static void init(void) {
  load_reminders();
  screen_list_show();

  //TODO show add screen if no reminder
  // if (get_reminder_count() == 0) {
  //   screen_add_show();
  // }
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
