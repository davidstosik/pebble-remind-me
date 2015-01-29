#include <pebble.h>
#include "list.h"
#include "reminder.h"

#if DEBUG
#include "debug.h"
#endif

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static MenuLayer *menu_layer;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // menu_layer
  menu_layer = menu_layer_create(GRect(0, 0, 144, 152));
  menu_layer_set_click_config_onto_window(menu_layer, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)menu_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(menu_layer);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  Reminder reminder;
  int position;
  time_t timestamp;
  switch(cell_index->section) {
    case 0:
      position = persist_reminder_get_first_future_position() + cell_index->row;
      persist_read_reminder(position, &reminder);
      timestamp = reminder.schedule_at;
      break;
    case 1:
    default:
      position = persist_reminder_get_first_past_position() + cell_index->row;
      persist_read_reminder(position, &reminder);
      timestamp = reminder.created_at;
      break;
  }
  char title[16];
  char subtitle[16];
  strftime(title, 16, "%Y/%m/%d", localtime(&timestamp));
  strftime(subtitle, 16, "%T", localtime(&timestamp));
  menu_cell_basic_draw(ctx, cell_layer, title, subtitle, NULL);
//   GRect bounds = layer_get_bounds(cell_layer);
}


static uint16_t get_num_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  switch(section_index) {
    case 0:
      return persist_reminder_count_future();
    case 1:
      return persist_reminder_count_past();
    default: return 0;
  }
}

static uint16_t get_num_sections(struct MenuLayer *menu_layer, void *callback_context) {
  int future = persist_reminder_count_future() > 0;
  int past = persist_reminder_count_past() > 0;
  return future + past;
}

// static int16_t get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
//   return 44;
// }

static int16_t get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return 16;
}

static void draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
  char * title = "";
  switch(section_index) {
    case 0:
      title = "Future";
      break;
    case 1:
      title = "Past";
      break;
  }
  menu_cell_basic_header_draw(ctx, cell_layer, title);
}

void show_list(void) {
  initialise_ui();
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .draw_row = draw_row,
    .get_num_rows = get_num_rows,
    .get_cell_height = NULL,
    .get_header_height = get_header_height,
    .draw_header = draw_header,
    .get_num_sections = get_num_sections,
    .select_click = NULL,
  });
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_list(void) {
  window_stack_remove(s_window, true);
}
