#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <persistence.h>

static Window *window;
static StatusBarLayer *status_bar_layer;
static ActionBarLayer *action_bar_layer;
static TextLayer *delay_text_layer;

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
  status_bar_layer_set_colors(status_bar_layer, APP_BG_COLOR, APP_FG_COLOR);

  // Change the status bar width to make space for the action bar
  int16_t width = bounds.size.w - ACTION_BAR_WIDTH;
  GRect frame = GRect(0, 0, width, STATUS_BAR_LAYER_HEIGHT);
  layer_set_frame(status_bar_layer_get_layer(status_bar_layer), frame);

  layer_add_child(window_layer, status_bar_layer_get_layer(status_bar_layer));

  action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
  action_bar_layer_set_background_color(action_bar_layer, APP_HL_COLOR);
  // action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, my_icon_previous, true);
  // action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, my_icon_next, true);
  action_bar_layer_add_to_window(action_bar_layer, window);

  int16_t height = bounds.size.h - STATUS_BAR_LAYER_HEIGHT;

  delay_text_layer = text_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, width, height/2));
  text_layer_set_text(delay_text_layer, "In 10min");
  text_layer_set_text_alignment(delay_text_layer, GTextAlignmentCenter);
  text_layer_set_font(delay_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(delay_text_layer));
}

static void window_unload(Window *window) {
  status_bar_layer_destroy(status_bar_layer);
  action_bar_layer_destroy(action_bar_layer);
  text_layer_destroy(delay_text_layer);
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
