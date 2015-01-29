#pragma once
#include <pebble.h>
#include "constants.h"
  
typedef enum {
  NEVER = 0,
  TEN_MIN,
  ONE_HR,
  SIX_HRS,
  ONE_DAY,
  SNOOZE_OPT_N,
} SnoozeOption;

/**
 * @return the list of available SnoozeOptions. 
 */
SnoozeOption* snooze_options();

/**
 * Return a label corresponding a SnoozeOption.
 * @param option a key from SnoozeOption enum.
 * @return the corresponding label.
 */
char * snooze_label(SnoozeOption option);

/**
 * Return the time associated to a snooze option.
 * @param option a key from SnoozeOption enum.
 * @param now a starting point from which to compute the snooze time.
 * @return the time at which snooze should happen.
 */
time_t snooze_time(SnoozeOption option, time_t now);

/**
 * Return the next SnoozeOption in the list.
 * @param current a SnoozeOption.
 * @return the next SnoozeOption in the list (or the first if end was reached).
 */
SnoozeOption next_snooze_opt_key(SnoozeOption current);