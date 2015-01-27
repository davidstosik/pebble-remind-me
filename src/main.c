#include <pebble.h>
#include "main.h"
#include "reminder.h"
#include "snooze_option.h"

// static int curr_snooze_opt;
static Reminder *reminder;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static GFont s_res_gothic_28_bold;
static GBitmap *s_res_bg_backbar;
static GBitmap *s_res_icon_x;
static GBitmap *s_res_bg_actionbar_1;
static GBitmap *s_res_bg_actionbar_2;
static GBitmap *s_res_icon_tick;
static GBitmap *s_res_icon_tag;
static GBitmap *s_res_icon_alarm;
static TextLayer *list_help;
static TextLayer *label_snooze;
static BitmapLayer *bg_backbar;
static BitmapLayer *icon_x;
static BitmapLayer *bg_actionbar_top;
static BitmapLayer *bg_actionbar_bottom;
static BitmapLayer *icon_tick;
static BitmapLayer *icon_tag;
static BitmapLayer *icon_alarm;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_bg_backbar = gbitmap_create_with_resource(RESOURCE_ID_BG_BACKBAR);
  s_res_icon_x = gbitmap_create_with_resource(RESOURCE_ID_ICON_X);
  s_res_bg_actionbar_1 = gbitmap_create_with_resource(RESOURCE_ID_BG_ACTIONBAR_1);
  s_res_bg_actionbar_2 = gbitmap_create_with_resource(RESOURCE_ID_BG_ACTIONBAR_2);
  s_res_icon_tick = gbitmap_create_with_resource(RESOURCE_ID_ICON_TICK);
  s_res_icon_tag = gbitmap_create_with_resource(RESOURCE_ID_ICON_TAG);
  s_res_icon_alarm = gbitmap_create_with_resource(RESOURCE_ID_ICON_ALARM);
  // list_help
  list_help = text_layer_create(GRect(60, 5, 60, 28));
  text_layer_set_text(list_help, "Long press for list");
  text_layer_set_text_alignment(list_help, GTextAlignmentRight);
  text_layer_set_font(list_help, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)list_help);
  
  // label_snooze
  label_snooze = text_layer_create(GRect(3, 65, 124, 33));
  text_layer_set_text(label_snooze, "Text layer");
  text_layer_set_text_alignment(label_snooze, GTextAlignmentCenter);
  text_layer_set_font(label_snooze, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)label_snooze);
  
  // bg_backbar
  bg_backbar = bitmap_layer_create(GRect(0, 3, 20, 36));
  bitmap_layer_set_bitmap(bg_backbar, s_res_bg_backbar);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bg_backbar);
  
  // icon_x
  icon_x = bitmap_layer_create(GRect(1, 12, 18, 18));
  bitmap_layer_set_bitmap(icon_x, s_res_icon_x);
  layer_add_child(window_get_root_layer(s_window), (Layer *)icon_x);
  
  // bg_actionbar_top
  bg_actionbar_top = bitmap_layer_create(GRect(124, 3, 20, 36));
  bitmap_layer_set_bitmap(bg_actionbar_top, s_res_bg_actionbar_1);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bg_actionbar_top);
  
  // bg_actionbar_bottom
  bg_actionbar_bottom = bitmap_layer_create(GRect(124, 52, 20, 97));
  bitmap_layer_set_bitmap(bg_actionbar_bottom, s_res_bg_actionbar_2);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bg_actionbar_bottom);
  
  // icon_tick
  icon_tick = bitmap_layer_create(GRect(125, 12, 18, 18));
  bitmap_layer_set_bitmap(icon_tick, s_res_icon_tick);
  layer_add_child(window_get_root_layer(s_window), (Layer *)icon_tick);
  
  // icon_tag
  icon_tag = bitmap_layer_create(GRect(125, 115, 18, 18));
  bitmap_layer_set_bitmap(icon_tag, s_res_icon_tag);
  layer_add_child(window_get_root_layer(s_window), (Layer *)icon_tag);
  
  // icon_alarm
  icon_alarm = bitmap_layer_create(GRect(125, 68, 18, 18));
  bitmap_layer_set_bitmap(icon_alarm, s_res_icon_alarm);
  layer_add_child(window_get_root_layer(s_window), (Layer *)icon_alarm);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(list_help);
  text_layer_destroy(label_snooze);
  bitmap_layer_destroy(bg_backbar);
  bitmap_layer_destroy(icon_x);
  bitmap_layer_destroy(bg_actionbar_top);
  bitmap_layer_destroy(bg_actionbar_bottom);
  bitmap_layer_destroy(icon_tick);
  bitmap_layer_destroy(icon_tag);
  bitmap_layer_destroy(icon_alarm);
  gbitmap_destroy(s_res_bg_backbar);
  gbitmap_destroy(s_res_icon_x);
  gbitmap_destroy(s_res_bg_actionbar_1);
  gbitmap_destroy(s_res_bg_actionbar_2);
  gbitmap_destroy(s_res_icon_tick);
  gbitmap_destroy(s_res_icon_tag);
  gbitmap_destroy(s_res_icon_alarm);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void update_snooze_label() {
  text_layer_set_text(label_snooze, get_snooze_options((time_t)0)[reminder->snooze_opt].label);
}

static void init_status() {
  reminder = new_reminder();
}

static void ui_custom_code() {
  bitmap_layer_set_compositing_mode(icon_x, GCompOpSet);
  bitmap_layer_set_compositing_mode(icon_tick, GCompOpSet);
  bitmap_layer_set_compositing_mode(icon_alarm, GCompOpSet);
  bitmap_layer_set_compositing_mode(icon_tag, GCompOpSet);
  
  update_snooze_label();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  reminder->snooze_opt = next_snooze_opt_key(reminder->snooze_opt);
  update_snooze_label();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - %s", "up_click_handler");
  push_reminder(reminder);
  free(reminder);
  window_stack_pop(true);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static bool list_help_bg = GColorWhite;
static void up_long_click_down_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text_color(list_help, list_help_bg);
  list_help_bg = list_help_bg == GColorBlack ? GColorWhite : GColorBlack;
  text_layer_set_background_color(list_help, list_help_bg);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, LONG_CLICK, up_long_click_down_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void show_main(void) {
  init_status();

  initialise_ui();
  ui_custom_code();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  
  window_stack_push(s_window, true);
}

void hide_main(void) {
  window_stack_remove(s_window, true);
}