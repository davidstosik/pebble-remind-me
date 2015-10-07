#include <pebble.h>
#include <reminder.h>
#include <persistence.h>

static Window *window;
static StatusBarLayer *status_bar_layer;
static ActionBarLayer *action_bar_layer;

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  struct Reminder * new_reminder = Reminder_create(600);
  reminders_add_reminder(new_reminder);
  window_stack_remove(window, true);
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
  // window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) my_previous_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  status_bar_layer = status_bar_layer_create();
  // status_bar_layer_set_separator_mode(status_bar_layer, StatusBarLayerSeparatorModeDotted);
  // Change the status bar width to make space for the action bar
  // int16_t width = layer_get_bounds(root_layer).size.w - ACTION_BAR_WIDTH;
  // GRect frame = GRect(0, 0, width, STATUS_BAR_LAYER_HEIGHT);
  // layer_set_frame(status_bar_layer_get_layer(status_bar), frame);
  // layer_add_child(root_layer, status_bar_layer_get_layer(status_bar));
  layer_add_child(window_layer, status_bar_layer_get_layer(status_bar_layer));

  action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
  // action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, my_icon_previous, true);
  // action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, my_icon_next, true);
  action_bar_layer_add_to_window(action_bar_layer, window);
}

static void window_unload(Window *window) {
  status_bar_layer_destroy(status_bar_layer);
  action_bar_layer_destroy(action_bar_layer);
  window_destroy(window);
}

void screen_add_show() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true);
}
