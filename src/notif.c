#include "notif.h"
#include <pebble.h>
  

// struct NotifWindowStruct {
//   Window *s_window;
// //   GFont s_res_bitham_30_black;
//   GBitmap *s_res_bg_backbar;
//   GBitmap *s_res_icon_x;
//   GBitmap *s_res_bg_actionbar_1;
//   GBitmap *s_res_icon_tick;
//   TextLayer *notif_text;
//   BitmapLayer *bg_backbar;
//   BitmapLayer *icon_x;
//   BitmapLayer *bg_actionbar_top;
//   BitmapLayer *icon_tick;
// };

// typedef struct NotifWindowStruct NotifWindow;

// static int stack_index = -1;
// static NotifWindow** notif_windows;
// static NotifWindow curr_notif_window;

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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "notif.c initialize_ui");
//   stack_index++;
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "stack_index: %d", stack_index);
//   notif_windows[stack_index] = malloc(sizeof(struct NotifWindowStruct)); 
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "destroy_ui");
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
//   stack_index--;
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "handle_window_unload");
  // FIXME: double free detected
  // Probably means if multiple notifications are triggered, then they share the same static!
  // Solutions:
  //  - when notification comes up, first clean previous notification, instead of stacking (:sad:)
  //  - when notification comes up, stack it on top, manage stack counter and array of Window and everything.
  //  - instantiate only once, destroy only once (count stack too?)
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "handle_window_unload");
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "stack_index = %d", stack_index);
//   if (--stack_index == 0) {
    destroy_ui();
//   }
}

static void ui_custom_code() {
  bitmap_layer_set_compositing_mode(icon_x, GCompOpSet);
  bitmap_layer_set_compositing_mode(icon_tick, GCompOpSet);
}

void show_notif(int id, int reason) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "METHOD - Enters %s", "show_notif");
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "stack_index = %d", stack_index);
//   if(stack_index++ == 0) {
    initialise_ui();
//   }
  ui_custom_code();
  
  vibes_long_pulse();
  light_enable_interaction();

  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_notif(void) {
  window_stack_remove(s_window, true);
}
