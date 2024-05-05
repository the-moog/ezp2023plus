#pragma once

#include <adwaita.h>
#include "chips_data_repository.h"

G_BEGIN_DECLS

#define ADW_TYPE_WINDOW_CHIPS_EDITOR (window_chips_editor_get_type())

G_DECLARE_FINAL_TYPE (WindowChipsEditor, window_chips_editor, ADW, WINDOW_CHIPS_EDITOR, AdwWindow)

WindowChipsEditor *window_chips_editor_new(ChipsDataRepository *repo);

G_END_DECLS