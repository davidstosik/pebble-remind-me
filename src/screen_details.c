#include <pebble.h>
#include <reminder.h>

static Window *window;
static TextLayer *text_layer;
static struct Reminder *reminder;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(bounds);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer_set_text(text_layer, reminder->message);
}

static void window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "window_unload() was called");
  text_layer_destroy(text_layer);
  window_destroy(window);
}

void screen_details_show(struct Reminder* _reminder) {
  reminder = _reminder;
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true);
}
