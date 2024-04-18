#include "chips_data_repository.h"

struct _ChipsDataRepository {
    GObject parent_instance;

    const char *file_path;
    ezp_chip_data *chips;
    int chips_count;
};

G_DEFINE_FINAL_TYPE(ChipsDataRepository, chips_data_repository, G_TYPE_OBJECT)

enum {
    CHIPS_LIST,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static guint
signal_new(const gchar *name, GType itype, GSignalFlags signal_flags,
           gpointer class_closure, GSignalAccumulator acc, gpointer accu_data,
           GSignalCMarshaller c_marshaller, GType return_type, guint n_params, ...) {
    guint signal_id;
    va_list var_args;
    va_start(var_args, n_params);
    signal_id = g_signal_new_valist(name, itype, signal_flags, class_closure, acc, accu_data,
                                    c_marshaller, return_type, n_params, var_args);
    va_end(var_args);
    return signal_id;
}

static void
chips_data_repository_class_init(ChipsDataRepositoryClass *klass) {

}

static void
chips_data_repository_init(ChipsDataRepository *self) {
    signals[CHIPS_LIST] = signal_new("chips-list", DATA_TYPE_CHIPS_DATA_REPOSITORY, G_SIGNAL_RUN_LAST, NULL,
                                     NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

ChipsDataRepository *
chips_data_repository_new(const char *file_path) {
    ChipsDataRepository *obj = g_object_new(DATA_TYPE_CHIPS_DATA_REPOSITORY, NULL);
    obj->file_path = file_path;
    return obj;
}

int
chips_data_repository_read(ChipsDataRepository *self) {
    if (self->chips) {
        free(self->chips);
        self->chips = NULL;
    }

    int ret = ezp_chips_data_read(&self->chips, self->file_path);
    if (ret >= 0) {
        self->chips_count = ret;

        chips_list *list = malloc(sizeof(chips_list));
        list->data = self->chips;
        list->length = ret;

        g_signal_emit_by_name(self, "chips-list", list);

        free(list);
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
chips_data_repository_add(ChipsDataRepository *self, ezp_chip_data *data) {
    g_warning("Not implemented");
    return 0;
}

int
chips_data_repository_edit(ChipsDataRepository *self, int index, ezp_chip_data *data) {
    g_warning("Not implemented");
    return 0;
}

int
chips_data_repository_delete(ChipsDataRepository *self, int index) {
    g_warning("Not implemented");
    return 0;
}