#pragma once

#include <glib-object.h>
#include "ezp_chips_data_file.h"

G_BEGIN_DECLS

#define EZP_TYPE_CHIPS_DATA_REPOSITORY (chips_data_repository_get_type())

G_DECLARE_FINAL_TYPE(ChipsDataRepository, chips_data_repository, EZP, CHIPS_DATA_REPOSITORY, GObject)

typedef struct {
    ezp_chip_data *data;
    guint length;
} chips_list;

ChipsDataRepository *
chips_data_repository_new(const char *file_path);

/**
 * Read data from file
 * @param self
 * @return 0 if success or some "EZP_*" error
 */
int
chips_data_repository_read(ChipsDataRepository *self);

/**
 * Save data to file
 * @param self
 * @return 0 if success or some "EZP_*" error
 */
int
chips_data_repository_save(ChipsDataRepository *self);

/**
 *
 * @param self
 * @param file_path
 * @return 0 if success or some "EZP_*" error
 */
int
chips_data_repository_import(ChipsDataRepository *self, const char *file_path);

/**
 *
 * @param self
 * @return chips_list
 */
chips_list
chips_data_repository_get_chips(ChipsDataRepository *self);

/**
 *
 * @param self
 * @param name
 * @return
 */
ezp_chip_data *
chips_data_repository_find_chip(ChipsDataRepository *self, const char *name);

/**
 *
 * @param self
 * @param data new item
 */
void
chips_data_repository_add(ChipsDataRepository *self, const ezp_chip_data *data);

/**
 *
 * @param self
 * @param index
 * @param data
 * @return TRUE - success, FALSE - index out of range
 */
gboolean
chips_data_repository_edit(ChipsDataRepository *self, guint index, const ezp_chip_data *data);

/**
 *
 * @param self
 * @param index
 * @return TRUE - success, FALSE - index out of range
 */
gboolean
chips_data_repository_delete(ChipsDataRepository *self, guint index);

G_END_DECLS