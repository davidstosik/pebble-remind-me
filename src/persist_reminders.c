#include <pebble.h>
#include "persist_reminders.h"

#include "constants.h"
#include "snooze_option.h"
#include "tools.h"

#if DEBUG
#include "debug.h"
#endif

int persist_reminders_get_size() {
  int result = 0;
  if (persist_exists(PERSIST_REMINDERS_COUNT_KEY)) {
    result = persist_read_int(PERSIST_REMINDERS_COUNT_KEY);
  }
  return result;
}

static int per_batch_quantity() {
  return (int)(PERSIST_DATA_MAX_LENGTH/sizeof_reminder());
}

static void batch_settings(int qty, int* per_batch_qty, int* batch_size, int* batch_count) {
  // Number of reminders per batch.
  *per_batch_qty = per_batch_quantity();
  // Batch size in bytes
  *batch_size = (*per_batch_qty)*sizeof_reminder();
  // Number of batch iterations.
  *batch_count = (int)(qty / *per_batch_qty);
}

void persist_reminders_destroy_all() {
  // Maximum number of slots storage could require.
  int max_slots = PERSIST_REMINDERS_MAX/per_batch_quantity();
  
  for(int slot = 0; slot < max_slots; slot++) {
    int address = PERSIST_REMINDERS_START_KEY + slot;
//     if (persist_exists(address)) {
      persist_delete(address);
//     }
  }

  persist_write_int(PERSIST_REMINDERS_COUNT_KEY, 0);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed all reminders!");
}

void persist_reminders_write(Reminder* reminders, int qty) {  
  int start_time = full_time_ms();

  int status = 0;
  
  int per_batch_qty, batch_size, batch_count;
  batch_settings(qty, &per_batch_qty, &batch_size, &batch_count);

  persist_reminders_destroy_all();
  
  int next_slot;
  for (next_slot = 0; next_slot < batch_count; next_slot++) {
    status = persist_write_data(
      PERSIST_REMINDERS_START_KEY + next_slot,
      &reminders[per_batch_qty * next_slot],
      batch_size
    );
    if (status < 0) APP_LOG(APP_LOG_LEVEL_ERROR, "Persisting data failed with status code %d.", status);
  }
  // Total number of batched items.
  int batched_qty = per_batch_qty * batch_count;
  // Remaining items count.
  int remainders = qty - batched_qty;
  if(remainders > 0) {
    status = persist_write_data(
      PERSIST_REMINDERS_START_KEY + next_slot,
      &reminders[batched_qty],
      remainders*sizeof_reminder()
    );
    if (status < 0) APP_LOG(APP_LOG_LEVEL_ERROR, "Persisting data failed with status code %d.", status);
  }
  
  persist_write_int(PERSIST_REMINDERS_COUNT_KEY, qty);

  APP_LOG(APP_LOG_LEVEL_DEBUG,
    "Saved %d reminders to storage in %dms, in batches of %d",
    qty, (int)(full_time_ms() - start_time), per_batch_qty
  );
}

void persist_reminders_read(Reminder* reminders) {
  int start_time = full_time_ms();

  int status = 0;
  
  int qty = persist_reminders_get_size();
  
  int per_batch_qty, batch_size, batch_count;
  batch_settings(qty, &per_batch_qty, &batch_size, &batch_count);
  
  int next_slot;
  for (next_slot = 0; next_slot < batch_count; next_slot++) {
    status = persist_read_data(
      PERSIST_REMINDERS_START_KEY + next_slot,
      &reminders[per_batch_qty * next_slot],
      batch_size
    );
    if (status < 0) APP_LOG(APP_LOG_LEVEL_ERROR, "Fetching data failed with status code %d.", status);
  }
  // Total number of batched items.
  int batched_qty = per_batch_qty * batch_count;
  // Remaining items count.
  int remainders = qty - batched_qty;
  if(remainders > 0) {
    status = persist_read_data(
      PERSIST_REMINDERS_START_KEY + next_slot,
      &reminders[batched_qty],
      remainders*sizeof_reminder()
    );
    if (status < 0) APP_LOG(APP_LOG_LEVEL_ERROR, "Fetching data failed with status code %d.", status);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG,
    "Read %d reminders from storage in %dms, in batches of %d",
    qty, (int)(full_time_ms() - start_time), per_batch_qty
  );
}
