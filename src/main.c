#include <pebble.h>
#include "main.h"
#include "reminder.h"

static char count_txt[BUF_LEN];
static char last_txt[BUF_LEN];

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static TextLayer *main_label;
static TextLayer *bottom_label;
static TextLayer *last_label;
static TextLayer *count_label;
static ActionBarLayer *actionbar;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  // main_label
  main_label = text_layer_create(GRect(0, 66, 124, 20));
  text_layer_set_text(main_label, "Press a button");
  text_layer_set_text_alignment(main_label, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)main_label);
  
  // bottom_label
  bottom_label = text_layer_create(GRect(0, 128, 124, 20));
  text_layer_set_text(bottom_label, "Down to show last");
  text_layer_set_text_alignment(bottom_label, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bottom_label);
  
  // last_label
  last_label = text_layer_create(GRect(0, 108, 124, 20));
  text_layer_set_text(last_label, "Text layer");
  text_layer_set_text_alignment(last_label, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)last_label);
  
  // count_label
  count_label = text_layer_create(GRect(0, 0, 60, 20));
  text_layer_set_text(count_label, "Count?");
  layer_add_child(window_get_root_layer(s_window), (Layer *)count_label);
  
  // actionbar
  actionbar = action_bar_layer_create();
  action_bar_layer_add_to_window(actionbar, s_window);
  action_bar_layer_set_background_color(actionbar, GColorBlack);
  layer_add_child(window_get_root_layer(s_window), (Layer *)actionbar);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(main_label);
  text_layer_destroy(bottom_label);
  text_layer_destroy(last_label);
  text_layer_destroy(count_label);
  action_bar_layer_destroy(actionbar);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

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
  text_layer_set_text(main_label, "Save and quit");
  add_reminder(reminder);
  save_reminders();
  window_stack_pop(true);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(main_label, "Up");
//   add_reminder();
  update_count();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(main_label, "Down");
  update_last();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void show_main(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  
  window_stack_push(s_window, true);
}

void hide_main(void) {
  window_stack_remove(s_window, true);
}