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

void persist_reminders_save(Reminder* reminders, int qty) {
  int start_time = full_time_ms();

//   int status = 0;
  int address;
  
  // Number of reminders per batch.
  int per_batch_qty = (int)(PERSIST_DATA_MAX_LENGTH/sizeof_reminder());
  // Number of batch iterations.
  int batch_count = (int)(qty/per_batch_qty);
  int next_slot;
  for (next_slot = 0; next_slot < batch_count; next_slot++) {
    address = PERSIST_REMINDERS_START_KEY + next_slot;
    if (persist_exists(address)) {
      persist_delete(address);
    }
//     status =
    persist_write_data(address, &reminders[per_batch_qty * next_slot], per_batch_qty*sizeof_reminder());
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying to write %dB into slot %d resulted in %d.", per_batch_qty*sizeof_reminder(), PERSIST_REMINDERS_START_KEY + next_slot, status);
  }
  // Total number of batched items.
  int batched_qty = per_batch_qty * batch_count;
  // Remaining item count.
  int remainders = qty - batched_qty;
  if(remainders > 0) {
    address = PERSIST_REMINDERS_START_KEY + next_slot;
    if (persist_exists(address)) {
      persist_delete(address);
    }
//     status =
    persist_write_data(PERSIST_REMINDERS_START_KEY + next_slot, &reminders[batched_qty], remainders*sizeof_reminder());
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying to write %dB into slot %d resulted in %d.", per_batch_qty*sizeof_reminder(), PERSIST_REMINDERS_START_KEY + next_slot, status);
    next_slot++;
  }
  
  int max_slots = PERSIST_REMINDERS_MAX/per_batch_qty;
  for (int i = next_slot; i < max_slots; i++) {
    persist_delete(PERSIST_REMINDERS_START_KEY + i);
  }
  
//   status =
  persist_write_int(PERSIST_REMINDERS_COUNT_KEY, qty);
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "PERSIST_REMINDERS_COUNT_KEY write status: %d", status);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved %d reminders to storage in %dms, in batches of %d", qty, (int)(full_time_ms() - start_time), per_batch_qty);
}

void persist_reminders_read(Reminder* reminders) {
  int start_time = full_time_ms();

//   int status = 0;
  
  int qty = persist_reminders_get_size();
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Reading count from storage: %d", (int)persist_read_int(PERSIST_REMINDERS_COUNT_KEY));

  // Number of reminders per batch.
  int per_batch_qty = (int)(PERSIST_DATA_MAX_LENGTH/sizeof_reminder());
  // Number of batch iterations.
  int batch_count = (int)(qty/per_batch_qty);
  int next_slot;
  for (next_slot = 0; next_slot < batch_count; next_slot++) {
//     status =
    persist_read_data(PERSIST_REMINDERS_START_KEY + next_slot, &reminders[per_batch_qty * next_slot], per_batch_qty*sizeof_reminder());
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying to read %dB into slot %d resulted in %d.", per_batch_qty*sizeof_reminder(), PERSIST_REMINDERS_START_KEY + next_slot, status);
  }
  // Total number of batched items.
  int batched_qty = per_batch_qty * batch_count;
  // Remaining item count.
  int remainders = qty - batched_qty;
  if(remainders > 0) {
//     status = 
    persist_read_data(PERSIST_REMINDERS_START_KEY + next_slot, &reminders[batched_qty], remainders*sizeof_reminder());
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying to read %dB into slot %d resulted in %d.", per_batch_qty*sizeof_reminder(), PERSIST_REMINDERS_START_KEY + next_slot, status);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Read %d reminders from storage in %dms, in batches of %d", qty, (int)(full_time_ms() - start_time), per_batch_qty);
}