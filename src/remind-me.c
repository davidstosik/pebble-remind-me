#include <pebble.h>
#include <reminder.h>
#include <persistence.h>
#include <screen_list.h>

static void init(void) {
  if (get_reminder_count() > 0) {
    load_reminders();
    screen_list_show();
  }
  else {
    // Show add reminder screen.
  }
}

static void deinit(void) {
  free_reminders();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
