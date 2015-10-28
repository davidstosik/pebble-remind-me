#include <pebble.h>
#include <reminder.h>

#define DATE_FONT          FONT_KEY_GOTHIC_18_BOLD
#define MESSAGE_FONT       FONT_KEY_GOTHIC_18
#define DATE_HEIGHT        20
#define TEXT_INSET         2
#define TEXT_BOTTOM_MARGIN 4

static Window *window;
static ScrollLayer *scroll_layer;
static TextLayer *message_layer;
static TextLayer *date_layer;
static char time_date_str[20];

static void window_load(Window *window) {
  struct Reminder* reminder = window_get_user_data(window);
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(scroll_layer, window);

  date_layer = text_layer_create(GRect(0, 0, bounds.size.w, DATE_HEIGHT));
  strftime(time_date_str, sizeof(time_date_str), "%Y/%m/%e %k:%M:%S", localtime(&(reminder->created_at)));
  text_layer_set_font(date_layer, fonts_get_system_font(DATE_FONT));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_text(date_layer, time_date_str);

  message_layer = text_layer_create(GRect(0, DATE_HEIGHT, bounds.size.w, 2000));
  text_layer_set_font(message_layer, fonts_get_system_font(MESSAGE_FONT));
  text_layer_set_text(message_layer, reminder->message);

  scroll_layer_add_child(scroll_layer, text_layer_get_layer(date_layer));
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(message_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));

  text_layer_enable_screen_text_flow_and_paging(message_layer, TEXT_INSET);

  GSize max_size = text_layer_get_content_size(message_layer);
  max_size.h += TEXT_BOTTOM_MARGIN;
  text_layer_set_size(message_layer, max_size);
  max_size.h += DATE_HEIGHT;
  scroll_layer_set_content_size(scroll_layer, max_size);
}

static void window_unload(Window *window) {
  text_layer_destroy(date_layer);
  text_layer_destroy(message_layer);
  scroll_layer_destroy(scroll_layer);
  window_destroy(window);
}

void screen_details_show(struct Reminder* reminder) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_user_data(window, reminder);

  window_stack_push(window, true);
}
