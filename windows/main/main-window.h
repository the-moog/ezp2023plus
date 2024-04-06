#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define MAIN_TYPE_MAIN_WINDOW (main_window_get_type())

G_DECLARE_FINAL_TYPE (MainWindow, main_window, MAIN, MAIN_WINDOW, AdwApplicationWindow)

MainWindow *main_window_new (GtkApplication *application);

G_END_DECLS
