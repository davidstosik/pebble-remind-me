#include <pebble.h>
#include "notif.h"
#include "reminder.h"
#include "snooze_option.h"

#if DEBUG
#include "debug.h"
#endif

static Reminder* reminder;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_bitham_30_black;
static GBitmap *s_res_bg_backbar;
static GBitmap *s_res_icon_x;
static GBitmap *s_res_bg_actionbar_1;
static GBitmap *s_res_icon_tick;
static TextLayer *notif_text;
static BitmapLayer *bg_backbar;
static BitmapLayer *icon_x;
static BitmapLayer *bg_actionbar_top;
static BitmapLayer *icon_tick;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, true);
  
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_res_bg_backbar = gbitmap_create_with_resource(RESOURCE_ID_BG_BACKBAR);
  s_res_icon_x = gbitmap_create_with_resource(RESOURCE_ID_ICON_X);
  s_res_bg_actionbar_1 = gbitmap_create_with_resource(RESOURCE_ID_BG_ACTIONBAR_1);
  s_res_icon_tick = gbitmap_create_with_resource(RESOURCE_ID_ICON_TICK);
  // notif_text
  notif_text = text_layer_create(GRect(0, 60, 144, 30));
  text_layer_set_text(notif_text, "NOTIF!");
  text_layer_set_text_alignment(notif_text, GTextAlignmentCenter);
  text_layer_set_font(notif_text, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)notif_text);
  
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
  
  // icon_tick
  icon_tick = bitmap_layer_create(GRect(125, 12, 18, 18));
  bitmap_layer_set_bitmap(icon_tick, s_res_icon_tick);
  layer_add_child(window_get_root_layer(s_window), (Layer *)icon_tick);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(notif_text);
  bitmap_layer_destroy(bg_backbar);
  bitmap_layer_destroy(icon_x);
  bitmap_layer_destroy(bg_actionbar_top);
  bitmap_layer_destroy(icon_tick);
  gbitmap_destroy(s_res_bg_backbar);
  gbitmap_destroy(s_res_icon_x);
  gbitmap_destroy(s_res_bg_actionbar_1);
  gbitmap_destroy(s_res_icon_tick);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  // FIXME: double free detected
  // Probably means if multiple notifications are triggered, then they share the same static!
  // Solutions:
  //  - when notification comes up, first clean previous notification, instead of stacking (:sad:)
  //  - when notification comes up, stack it on top, manage stack counter and array of Window and everything.
  //  - instantiate only once, destroy only once (count stack too?)
  destroy_ui();
//   persist_push_reminder(reminder);
  free(reminder);
}

static void init_status() {
  // FIXME: for now we suppose a notification is always triggered by first notification (and it should be).
  reminder = malloc(sizeof_reminder());
//   persist_pull_reminder(0, reminder);
  reminder->snooze_opt = NEVER;
}

static void ui_custom_code() {
  bitmap_layer_set_compositing_mode(icon_x, GCompOpSet);
  bitmap_layer_set_compositing_mode(icon_tick, GCompOpSet);
}

static void click_config_provider(void *context) {
}

void show_notif(int id, int reason) {
  init_status();
  initialise_ui();
  ui_custom_code();
  vibes_long_pulse();
  light_enable_interaction();

  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  window_stack_push(s_window, true);
}

void hide_notif(void) {
  window_stack_remove(s_window, true);
}
