/**
 * Mize part entry for Vicinae.
 *
 * Builds into libmize_part_vicinae.so and exports part_vicinae().
 */

#include "server.hpp"

#include <QMetaObject>

#include <mize.h>

#include "navigation-controller.hpp"

static DATA *method_open(DATA *args) {
  (void)args;

  DATA *nav_data = get_config("vicinae.nav");
  if (!nav_data) {
    mize_write_stdout("[vicinae] open: vicinae.nav not set yet\n");
    return data_new();
  }

  auto *nav = static_cast<NavigationController *>(data_as_native(nav_data));
  data_free(nav_data);

  if (!nav) {
    mize_write_stdout("[vicinae] open: nav handle is null\n");
    return data_new();
  }

  QMetaObject::invokeMethod(nav, "showWindow", Qt::QueuedConnection);
  return data_new();
}

static DATA *vicinae_start(DATA *args) {
  (void)args;
  mize_write_stdout("[vicinae] starting Qt thread\n");
  ServerLaunchOptions opts;
  opts.open = false;
  startServer(opts);
  return data_new();
}

#if defined(_WIN32)
#define PART __declspec(dllexport)
#else
#define PART __attribute__((visibility("default")))
#endif

extern "C" PART void part_vicinae(void) {
  mize_write_stdout("[vicinae] part_vicinae()\n");

  opt("vicinae.nav");
  method("vicinae.open", "() -> ()", method_open);
  thread("vicinae.main", vicinae_start);
}
