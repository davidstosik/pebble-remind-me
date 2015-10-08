#include <pebble.h>
#include <constants.h>
#include <reminder.h>
#include <persistence.h>
#include <screen_add.h>

static Window *window;
static MenuLayer *s_menu_layer;
static StatusBarLayer *status_bar_layer;
static char menu_title_str[50];
static char menu_subtitle_str[50];

static void format_time(time_t timestamp, char * string) {
  strftime(string, 17, "%k:%M %e-%m-%Y", localtime(&timestamp));
}

static void draw_add_button_menu_item(GContext *ctx, const Layer *cell_layer) {
  GRect box = layer_get_frame(cell_layer);
  graphics_draw_text(ctx, "+", fonts_get_system_font(FONT_KEY_GOTHIC_28), box, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void draw_reminder_menu_item(GContext *ctx, const Layer *cell_layer, struct Reminder *reminder) {
  format_time(reminder->created_at, menu_title_str);
  format_time(reminder->remind_at, menu_subtitle_str);
  menu_cell_basic_draw(ctx, cell_layer, menu_title_str, menu_subtitle_str, NULL);
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  if (cell_index->section != 0) { return; }
  if (cell_index->row > get_reminder_count()) { return; }
  else if (cell_index->row == 0) {
    draw_add_button_menu_item(ctx, cell_layer);
  }
  else {
    struct Reminder * reminder = get_reminders()[cell_index->row - 1];
    draw_reminder_menu_item(ctx, cell_layer, reminder);
  }
}

static uint16_t get_num_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  switch(section_index) {
    case 0: return get_reminder_count() + 1;
    default: return 0;
  }
}

static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if (cell_index->section != 0) { return; }
  if (cell_index->row > get_reminder_count()) { return; }
  else if (cell_index->row == 0) {
    screen_add_show();
  }
  else {
    // show reminder details/edit action menu
  }
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
    .get_num_rows = get_num_rows,
    .select_click = select_click
  });
  layer_add_child(window_get_root_layer(window), (Layer *)s_menu_layer);
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  status_bar_layer_destroy(status_bar_layer);
  window_destroy(window);
}

static void window_appear(Window *window) {
  menu_layer_reload_data(s_menu_layer);
}

void screen_list_show() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
  });

  window_stack_push(window, true);
}