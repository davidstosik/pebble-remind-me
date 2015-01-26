#include <pebble.h>

#include "constants.h"
#include "reminder.h"
#include "notif.h"

static Window *window;
static TextLayer *text_layer;
static TextLayer *help_label;
static TextLayer *last_label;
static TextLayer *count_label;


static char count_txt[BUF_LEN];
static char last_txt[BUF_LEN];

static void update_count() {
  snprintf(count_txt, BUF_LEN, "Count:%d", reminders_count);
  text_layer_set_text(count_label, count_txt);
}

static void update_last() {
  if (reminders_count > 0) {
    time_t last = reminders[reminders_count-1].created_at;
    strftime(last_txt, BUF_LEN, "%T", localtime(&last));
  } else {
    snprintf(last_txt, BUF_LEN, "empty...");
  }
  
  text_layer_set_text(last_label, last_txt);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Save and quit");
  add_reminder(reminder);
  save_reminders();
  window_stack_pop(true);
}

// static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//   text_layer_set_text(text_layer, "Save and add new");
//   count = add_reminder(reminders, curr_rmndr, count);
//   init_reminder(&curr_rmndr);
// }

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
//   add_reminder();
  update_count();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
  update_last();
}

// static void long_up_down_handler(ClickRecognizerRef recognizer, void *context) {
//   text_layer_set_text(text_layer, "Long (list)");
// }

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
//   window_long_click_subscribe(BUTTON_ID_SELECT, LONG_CLICK, select_long_click_handler, NULL);
//   window_long_click_subscribe(BUTTON_ID_UP, LONG_CLICK, long_up_down_handler, NULL);
//   window_long_click_subscribe(BUTTON_ID_DOWN, LONG_CLICK, long_up_down_handler, NULL);
}

static void window_main_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "window_main_load");
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  help_label = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 20 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(help_label, "Down to show last");
  text_layer_set_text_alignment(help_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(help_label));
  
  last_label = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 40 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(last_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(last_label));
  
  count_label = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 60, 20 } });
  text_layer_set_text(count_label, "Count?");
  text_layer_set_text_alignment(count_label, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(count_label));
  
  update_count();
}

static void window_main_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "window_main_unload");
  text_layer_destroy(text_layer);
  text_layer_destroy(help_label);
  text_layer_destroy(last_label);
  text_layer_destroy(count_label);
}

static void init_window_main() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "init_window_main");
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_main_load,
    .unload = window_main_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "init");
  load_reminders();
  
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    show_notif();
  } else {
    init_reminder(&reminder);
    init_window_main();
  }
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}