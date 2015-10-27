#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <reminder_list.h>
#include <persistence.h>
#include <timestamp_format.h>
#include <screen_details.h>

static Window *window;
static MenuLayer *s_menu_layer;
static StatusBarLayer *status_bar_layer;
static ActionMenu *reminder_action_menu;
static ActionMenuLevel *reminder_root_level;
static char menu_title_str[50];
static char menu_subtitle_str[50];
static DictationSession *s_dictation_session;

static void draw_add_button_menu_item(GContext *ctx, const Layer *cell_layer) {
  GRect box = layer_get_frame(cell_layer);
  graphics_draw_text(ctx, "+", fonts_get_system_font(FONT_KEY_GOTHIC_28), box, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void draw_reminder_menu_item(GContext *ctx, const Layer *cell_layer, struct Reminder *reminder) {
  format_timestamp(reminder->remind_at, menu_subtitle_str);
  menu_cell_basic_draw(ctx, cell_layer, reminder->message, menu_subtitle_str, NULL);
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  switch(cell_index->section) {
    case 0:
      draw_add_button_menu_item(ctx, cell_layer);
      break;
    case 1:
      if (cell_index->row >= ReminderList_size(all_reminders)) { return; }
      struct Reminder* reminder = ReminderList_get_reminder_at(all_reminders, cell_index->row);
      draw_reminder_menu_item(ctx, cell_layer, reminder);
      break;
    default:
      return;
  }
}

static uint16_t get_num_sections(struct MenuLayer *menu_layer, void *callback_context) {
  return 2;
}

static uint16_t get_num_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  switch(section_index) {
    case 0: return 1;
    case 1: return ReminderList_size(all_reminders);
    default: return 0;
  }
}

static void reminder_action_menu_delete(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
  struct Reminder deleted;
  ReminderList_delete_at(all_reminders, selected.row, &deleted);
}

static void reminder_action_menu_details(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  MenuIndex selected = menu_layer_get_selected_index(s_menu_layer);
  struct Reminder* reminder = ReminderList_get_reminder_at(all_reminders, selected.row	);

  screen_details_show(reminder);
}

static void show_action_menu(int clicked_index) {
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

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Dictation status: %d", (int)status);
  APP_LOG(APP_LOG_LEVEL_INFO, "Dictation message: %s", transcription);
  switch(status) {
    case 0: {
      struct Reminder new_reminder = Reminder_create(transcription);
      ReminderList_insert_sorted(all_reminders, new_reminder);
      menu_layer_reload_data(s_menu_layer);
      break;
    }
    case 1:
      // Dismissed.
      break;
    default:
      //error;
      break;
  }
  free(transcription);
}

static void init_dictation() {
  if (!s_dictation_session) {
    s_dictation_session = dictation_session_create(REMINDER_MESSAGE_MAX_LENGTH, dictation_session_callback, NULL);
  }
  dictation_session_start(s_dictation_session);
}

static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch(cell_index->section) {
    case 0:
      init_dictation();
      break;
    case 1:
      show_action_menu(cell_index->row);
      break;
    default:
      return;
  }
}

static void init_reminder_action_menu() {
  reminder_root_level = action_menu_level_create(2);
  action_menu_level_add_action(reminder_root_level, "Details", reminder_action_menu_details, NULL);
  action_menu_level_add_action(reminder_root_level, "Delete", reminder_action_menu_delete, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  status_bar_layer = status_bar_layer_create();
  status_bar_layer_set_colors(status_bar_layer, APP_BG_COLOR, APP_FG_COLOR);
  layer_add_child(window_layer, status_bar_layer_get_layer(status_bar_layer));

  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, bounds.size.w, bounds.size.h - STATUS_BAR_LAYER_HEIGHT));
  menu_layer_set_highlight_colors(s_menu_layer, APP_HL_COLOR, APP_BG_COLOR);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .draw_row = draw_row,
    .get_num_sections = get_num_sections,
    .get_num_rows = get_num_rows,
    .select_click = select_click
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  init_reminder_action_menu();
}

static void window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "window_unload() was called");
  menu_layer_destroy(s_menu_layer);
  status_bar_layer_destroy(status_bar_layer);
  window_destroy(window);
  action_menu_hierarchy_destroy(reminder_root_level, NULL, NULL);
  dictation_session_destroy(s_dictation_session);
}

static void window_appear(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "window_appear() was called");
  menu_layer_reload_data(s_menu_layer);
}

void screen_list_show(bool start_dictation) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
  });

  window_stack_push(window, true);

  if (start_dictation) {
    init_dictation();
  }
}
