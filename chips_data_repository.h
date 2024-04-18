#pragma once

#include <glib-object.h>
#include "ezp_chips_data_file.h"

G_BEGIN_DECLS

#define DATA_TYPE_CHIPS_DATA_REPOSITORY (chips_data_repository_get_type())

G_DECLARE_FINAL_TYPE(ChipsDataRepository, chips_data_repository, DATA, CHIPS_DATA_REPOSITORY, GObject)

typedef struct {
    ezp_chip_data *data;
    int length;
} chips_list;

ChipsDataRepository *chips_data_repository_new(const char *file_path);

/**
 * Read data from file
 * @param self
 * @return 0 if success or some "EZP_*" error
 */
int chips_data_repository_read(ChipsDataRepository *self);

/**
 * Save data to file
 * @param self
 * @return 0 if success or some "EZP_*" error
 */
int chips_data_repository_save(ChipsDataRepository *self);

int chips_data_repository_add(ChipsDataRepository *self, ezp_chip_data *data);

int chips_data_repository_edit(ChipsDataRepository *self, int index, ezp_chip_data *data);

int chips_data_repository_delete(ChipsDataRepository *self, int index);

G_END_DECLS