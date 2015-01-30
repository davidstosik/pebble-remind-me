#pragma once
#include <pebble.h>

uint32_t full_time_ms();

/**
 * ISO compliant realloc().
 */
void * iso_realloc(void *ptr, size_t size);