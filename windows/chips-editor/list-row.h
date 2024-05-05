#pragma once

#include <glib-object.h>
#include <ezp_chips_data_file.h>

G_BEGIN_DECLS

#define EZP_TYPE_CHIPS_EDITOR_LIST_ROW (chips_editor_list_row_get_type())

G_DECLARE_FINAL_TYPE (ChipsEditorListRow, chips_editor_list_row, EZP, CHIPS_EDITOR_LIST_ROW, GObject)

ChipsEditorListRow *
chips_editor_list_row_new(const ezp_chip_data *data);

void
chips_editor_list_row_set_flash_type(ChipsEditorListRow *self, const char *flash_type);

void
chips_editor_list_row_set_manufacturer(ChipsEditorListRow *self, const char *manufacturer);

void
chips_editor_list_row_set_name(ChipsEditorListRow *self, const char *name);

const char *
chips_editor_list_row_get_flash_type(ChipsEditorListRow *self);

const char *
chips_editor_list_row_get_manufacturer(ChipsEditorListRow *self);

const char *
chips_editor_list_row_get_name(ChipsEditorListRow *self);

G_END_DECLS