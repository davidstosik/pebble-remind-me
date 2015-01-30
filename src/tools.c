#include <pebble.h>
#include "tools.h"

uint32_t full_time_ms() {
  time_t   sec = 0;
  uint16_t ms  = 0;
  time_ms(&sec, &ms);
  return 1000 * sec + ms;
}

void* iso_realloc(void* ptr, size_t size) {
  if (ptr != NULL) {
    return realloc(ptr, size);
  } else {
    return malloc(size);
  }
}