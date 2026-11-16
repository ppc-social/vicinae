/**
 * Minimal startServer for the mize part.
 *
 * Grow this toward the full implementation in src/server/src/server.cpp.
 * Goal for now: bring up Qt + launcher window plumbing so method("vicinae.open")
 * can show the prompt.
 */

#include "server.hpp"

#include "builtins/root/root-command.hpp"
#include "common/context.hpp"
#include "config/config.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/windows/launcher-window.hpp"
#include "ui/windows/overlay-controller.hpp"
#include "vicinae.hpp"

#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>
#include <cstring>

#include <mize.h>

int startServer(const ServerLaunchOptions &launchOpts) {
  (void)launchOpts;

  int argc = 1;
  static char *argv[] = {strdup("vicinae"), nullptr};
  QGuiApplication qapp(argc, argv);
  QGuiApplication::setApplicationName("vicinae");
  QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
  QQuickStyle::setStyle(QStringLiteral("Basic"));
  QGuiApplication::setQuitOnLastWindowClosed(false);

  Omnicast::ensureDirectories();

  ServiceRegistry serviceRegistry;
  auto *registry = ServiceRegistry::instance();

  auto configPath = Omnicast::configDir() / "settings.json";
  registry->setConfig(std::make_unique<config::Manager>(configPath));

  ApplicationContext ctx;
  ctx.navigation = std::make_unique<NavigationController>(ctx);
  ctx.overlay = std::make_unique<OverlayController>(&ctx);
  ctx.settings = std::make_unique<SettingsController>(ctx);
  ctx.services = registry;

  // Publish nav handle for vicinae.open (absolute opt name)
  set_config("vicinae.nav", data_from_native(static_cast<void *>(ctx.navigation.get())));

  LauncherWindow const qmlWindow(ctx);
  ctx.navigation->launch(std::make_shared<RootCommand>());

  mize_write_stdout("[vicinae] minimal startServer ready\n");
  return qApp->exec();
}
