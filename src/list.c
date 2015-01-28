#include <pebble.h>
#include "list.h"
#include "reminder.h"
#include "log.h"

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
  persist_read_reminder(cell_index->row, &reminder);
  char label[32];
  if (reminder.schedule_at > 0) {
    strftime(label, 32, "%Y/%m/%d - %T", localtime(&(reminder.schedule_at)));
  } else {
    strcpy(label, "Past");
  }
//   menu_cell_basic_draw(ctx, cell_layer, label, NULL, NULL);
  

//   LibraryMenu *menu = (LibraryMenu*)callback_context;
//   int16_t pos = cell_index->row - menu->current_entry_offset;
//   if(pos >= MENU_CACHE_COUNT || pos < 0) return;
  //menu_cell_basic_draw(ctx, cell_layer, menu->menu_entries[pos], NULL, NULL);
  graphics_context_set_text_color(ctx, GColorBlack);
  GRect bounds = cell_layer->bounds;
  bounds.origin.x += 5;
  bounds.origin.y -= 4;
  bounds.size.w -= 5;
  graphics_text_draw(ctx, label, s_res_gothic_14, bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static uint16_t get_num_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return persist_reminder_count();
}

static int16_t get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  return 28;
}

void show_list(void) {
  initialise_ui();
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .draw_row = draw_row,
    .get_num_rows = get_num_rows,
    .get_cell_height = get_cell_height,
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
