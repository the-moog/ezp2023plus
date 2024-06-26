#include "chips-data-repository.h"

struct _ChipsDataRepository {
    GObject parent_instance;

    const char *file_path;
    ezp_chip_data *chips;
    guint chips_count;
};

G_DEFINE_FINAL_TYPE(ChipsDataRepository, chips_data_repository, G_TYPE_OBJECT)

enum {
    CHIPS_LIST,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
chips_data_repository_class_init(G_GNUC_UNUSED ChipsDataRepositoryClass *klass) {

}

static void
chips_data_repository_init(G_GNUC_UNUSED ChipsDataRepository *self) {
    signals[CHIPS_LIST] = g_signal_new("chips-list", EZP_TYPE_CHIPS_DATA_REPOSITORY, G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                       NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

ChipsDataRepository *
chips_data_repository_new(const char *file_path) {
    ChipsDataRepository *obj = g_object_new(EZP_TYPE_CHIPS_DATA_REPOSITORY, NULL);
    obj->file_path = file_path;
    obj->chips = NULL;
    obj->chips_count = 0;
    return obj;
}

void
emit_chips_list(ChipsDataRepository *self) {
    chips_list list = {self->chips, self->chips_count};
    g_signal_emit_by_name(self, "chips-list", &list);
}

int
chips_data_repository_read(ChipsDataRepository *self) {
    if (self->chips) {
        free(self->chips);
        self->chips = NULL;
        self->chips_count = 0;
    }

    int ret = ezp_chips_data_read(&self->chips, self->file_path);
    if (ret >= 0) {
        self->chips_count = ret;
        emit_chips_list(self);
        return 0;
    } else {
        return ret;
    }
}

int
chips_data_repository_save(ChipsDataRepository *self) {
    if (!self->chips) {
        return 0;
    }
    return ezp_chips_data_write(self->chips, self->chips_count, self->file_path);
}

int
chips_data_repository_import(ChipsDataRepository *self, const char *file_path) {
    ezp_chip_data *data = NULL;
    int ret = ezp_chips_data_read(&data, file_path);
    if (ret >= 0) {
        if (ret == 0) return 0;
        if (!self->chips) {
            self->chips = data;
            self->chips_count = ret;
        } else {
            self->chips = g_realloc_n(self->chips, self->chips_count + ret, sizeof(ezp_chip_data));
            memcpy(self->chips + self->chips_count, data, sizeof(ezp_chip_data) * ret);
            self->chips_count += ret;
            free(data);
        }
        emit_chips_list(self);
        return 0;
    } else {
        return ret;
    }
}

chips_list
chips_data_repository_get_chips(ChipsDataRepository *self) {
    chips_list list = {self->chips, self->chips_count};
    return list;
}

ezp_chip_data *
chips_data_repository_find_chip(ChipsDataRepository *self, const char *name) {
    for (size_t i = 0; i < self->chips_count; ++i)
        if (!strcmp(self->chips[i].name, name))
            return &self->chips[i];
    return NULL;
}

void
chips_data_repository_add(ChipsDataRepository *self, const ezp_chip_data *data) {
    if (!self->chips) {
        self->chips = malloc(sizeof(ezp_chip_data));
        self->chips_count = 1;
        memcpy(self->chips, data, sizeof(ezp_chip_data));
        emit_chips_list(self);
    } else {
        ezp_chip_data *new_chips = g_realloc_n(self->chips, self->chips_count + 1, sizeof(ezp_chip_data));
        memcpy(&new_chips[self->chips_count], data, sizeof(ezp_chip_data));
        self->chips = new_chips;
        self->chips_count++;
        emit_chips_list(self);
    }
}

gboolean
chips_data_repository_edit(ChipsDataRepository *self, guint index, const ezp_chip_data *data) {
    if (index < self->chips_count) {
        memcpy(&self->chips[index], data, sizeof(ezp_chip_data));
        emit_chips_list(self);
        return TRUE;
    }
    return FALSE;
}

gboolean
chips_data_repository_delete(ChipsDataRepository *self, guint index) {
    if (index >= self->chips_count) return FALSE;

    if (self->chips_count == 1) {
        free(self->chips);
        self->chips = NULL;
        self->chips_count = 0;
        emit_chips_list(self);
    } else {
        void *dest = &self->chips[index];
        void *src = dest + sizeof(ezp_chip_data);
        guint count = self->chips_count - index;

        if (count > 0) memcpy(dest, src, self->chips_count - index - 1);

        ezp_chip_data *new_chips = g_realloc_n(self->chips, self->chips_count - 1, sizeof(ezp_chip_data));
        self->chips = new_chips;
        self->chips_count--;
        emit_chips_list(self);
    }
    return TRUE;
}