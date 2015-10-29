#include <pebble.h>
#include <constants.h>

#include "pebble_process_info.h"
extern const PebbleProcessInfo __pbl_app_info;

static Version app_version() {
  return __pbl_app_info.process_version;
}

static Version load_app_version_from_memory() {
  Version result = {
    .major = 0,
    .minor = 0,
  };

  if (persist_exists(PERSIST_APP_VERSION_KEY)) {
    persist_read_data(
      PERSIST_APP_VERSION_KEY,
      &result,
      sizeof(result)
    );
  }

  return result;
}

static void persist_app_version_to_memory(Version version) {
  persist_write_data(
    PERSIST_APP_VERSION_KEY,
    &version,
    sizeof(version)
  );
  APP_LOG(APP_LOG_LEVEL_INFO, "- Saved memory version as %i.%i.", version.major, version.minor);

}

void check_updates() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Checking updates.");

  Version memory_version = load_app_version_from_memory();
  APP_LOG(APP_LOG_LEVEL_INFO, "- Current version is %i.%i.", memory_version.major, memory_version.minor);


  // Add calls to update functions here? Introspection to find if there are functions?

  persist_app_version_to_memory(app_version());
}
