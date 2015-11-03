#include <pebble.h>
#include <reminder.h>
#include <reminder_list.h>
#include <constants.h>
#include <persistence.h>

#define DATE_FONT          FONT_KEY_GOTHIC_24_BOLD
#define MESSAGE_FONT       FONT_KEY_GOTHIC_24
#define DATE_HEIGHT        48
#define TEXT_INSET         2
#define TEXT_BOTTOM_MARGIN 4

static Window *window;
static ScrollLayer *scroll_layer;
static ContentIndicator * content_indicator;
static TextLayer *message_layer;
static TextLayer *date_layer;
static char time_date_str[20];
static ActionBarLayer *action_bar_layer;
static GBitmap* icon_up;
static GBitmap* icon_down;
static GBitmap* icon_ellipsis;
static ActionMenu *reminder_action_menu;
static ActionMenuLevel *reminder_root_level;

static void show_action_menu() {
  ActionMenuConfig config = (ActionMenuConfig){
    .root_level = reminder_root_level,
    .colors = {
      .background = APP_HL_COLOR,
      .foreground = APP_FG_COLOR,
    },
    .align = ActionMenuAlignCenter
    // .align = ,
    // .context = ,
    // .did_close = ,
    // .will_close = ,
  };
  reminder_action_menu = action_menu_open(&config);
}

static void reminder_action_menu_delete(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  struct Reminder* reminder = window_get_user_data(window);
  ReminderList_delete(all_reminders, reminder);
  window_stack_remove(window, false);
}

static void init_reminder_action_menu() {
  reminder_root_level = action_menu_level_create(1);
  // action_menu_level_add_action(reminder_root_level, "Details", reminder_action_menu_details, NULL);
  action_menu_level_add_action(reminder_root_level, "Delete", reminder_action_menu_delete, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_action_menu();
}

static void update_direction_arrows() {
  if (!content_indicator_get_content_available(content_indicator, ContentIndicatorDirectionDown)) {
     action_bar_layer_clear_icon(action_bar_layer, BUTTON_ID_DOWN);
  }
  else {
    action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, icon_down);
  }
  if (!content_indicator_get_content_available(content_indicator, ContentIndicatorDirectionUp)) {
     action_bar_layer_clear_icon(action_bar_layer, BUTTON_ID_UP);
  }
  else {
    action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, icon_up);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  scroll_layer_scroll_up_click_handler(recognizer, scroll_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  scroll_layer_scroll_down_click_handler(recognizer, scroll_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 40, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_click_handler);

  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void content_offset_changed_handler(struct ScrollLayer *scroll_layer, void *context) {
  update_direction_arrows();
}

static void window_load(Window *window) {
  struct Reminder* reminder = window_get_user_data(window);
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  icon_up = gbitmap_create_with_resource(RESOURCE_ID_ICON_UP);
  icon_down = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN);
  icon_ellipsis = gbitmap_create_with_resource(RESOURCE_ID_ICON_ELLIPSIS);

  action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
  action_bar_layer_set_background_color(action_bar_layer, APP_HL_COLOR);
  action_bar_layer_set_icon_press_animation(action_bar_layer, BUTTON_ID_UP, ActionBarLayerIconPressAnimationMoveUp);
  action_bar_layer_set_icon_press_animation(action_bar_layer, BUTTON_ID_DOWN, ActionBarLayerIconPressAnimationMoveDown);
  // action_bar_layer_set_icon_press_animation(action_bar_layer, BUTTON_ID_SELECT, ActionBarLayerIconPressAnimationMoveDown);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, icon_ellipsis);
  action_bar_layer_add_to_window(action_bar_layer, window);

  bounds.size.w -= ACTION_BAR_WIDTH;

  scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_callbacks(scroll_layer, (ScrollLayerCallbacks){
    // .click_config_provider = click_config_provider,
    .content_offset_changed_handler = content_offset_changed_handler
  });
  // scroll_layer_set_click_config_onto_window(scroll_layer, window);
  content_indicator = scroll_layer_get_content_indicator(scroll_layer);

  date_layer = text_layer_create((GRect){.origin = bounds.origin, .size = GSize(bounds.size.w, DATE_HEIGHT)});
  strftime(time_date_str, sizeof(time_date_str), "%Y/%m/%d\n%k:%M:%S", localtime(&(reminder->created_at)));
  text_layer_set_font(date_layer, fonts_get_system_font(DATE_FONT));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_text(date_layer, time_date_str);

  bounds.origin.y += DATE_HEIGHT;
  bounds.origin.x += TEXT_INSET;
  bounds.size.w -= 2 * TEXT_INSET;
  bounds.size.h = 2000;
  message_layer = text_layer_create(bounds);
  text_layer_set_font(message_layer, fonts_get_system_font(MESSAGE_FONT));
  text_layer_set_text(message_layer, reminder->message);

  scroll_layer_add_child(scroll_layer, text_layer_get_layer(date_layer));
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(message_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));

  // Only for Round?
  // text_layer_enable_screen_text_flow_and_paging(message_layer, TEXT_INSET);

  GSize max_size = text_layer_get_content_size(message_layer);
  max_size.h += TEXT_BOTTOM_MARGIN;
  text_layer_set_size(message_layer, max_size);
  max_size.h += DATE_HEIGHT;
  scroll_layer_set_content_size(scroll_layer, max_size);

  update_direction_arrows();
  init_reminder_action_menu();
}

static void window_unload(Window *window) {
  action_menu_hierarchy_destroy(reminder_root_level, NULL, NULL);
  text_layer_destroy(date_layer);
  text_layer_destroy(message_layer);
  content_indicator_destroy(content_indicator);
  scroll_layer_destroy(scroll_layer);
  action_bar_layer_destroy(action_bar_layer);
  gbitmap_destroy(icon_up);
  gbitmap_destroy(icon_down);
  gbitmap_destroy(icon_ellipsis);
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
