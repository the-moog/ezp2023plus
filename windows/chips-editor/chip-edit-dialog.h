#pragma once

#include <adwaita.h>
#include "chips-data-repository.h"

G_BEGIN_DECLS

#define EZP_TYPE_DIALOG_CHIPS_EDIT (dialog_chips_edit_get_type())

G_DECLARE_FINAL_TYPE (DialogChipsEdit, dialog_chips_edit, EZP, DIALOG_CHIPS_EDIT, AdwDialog)

DialogChipsEdit *
dialog_chips_edit_new(ChipsDataRepository *repo, guint open_index);

G_END_DECLS
