#pragma once

#include <adwaita.h>
#include "chips_data_repository.h"

G_BEGIN_DECLS

#define EZP_TYPE_WINDOW_MAIN (window_main_get_type())

G_DECLARE_FINAL_TYPE (WindowMain, window_main, EZP, WINDOW_MAIN, AdwApplicationWindow)

WindowMain *
window_main_new(GtkApplication *application, ChipsDataRepository *repo);

G_END_DECLS
