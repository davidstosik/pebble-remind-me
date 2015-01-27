#include <pebble.h>

#define NEVER      0
#define TEN_MIN    1
#define ONE_HR     2
#define SIX_HRS    3
#define ONE_DAY    4
  
#define SNOOZE_OPT_N 5
  
#define SNOOZE_LABEL_MAX_SIZE 64

struct SnoozeOptionStruct {
  char label[SNOOZE_LABEL_MAX_SIZE];
  time_t time;
};

typedef struct SnoozeOptionStruct SnoozeOption;

SnoozeOption* get_snooze_options(time_t);
int next_snooze_opt_key(int);