#include "gtk_string_list_extension.h"

size_t gtk_string_list_size(GtkStringList *self) {
    for (int i = 0;; ++i) {
        if (!gtk_string_list_get_string(self, i)) return i;
    }
}

int gtk_string_list_contains(GtkStringList *self, const char *str) {
    const char *ret = NULL;
    for (int i = 0;; ++i) {
        ret = gtk_string_list_get_string(self, i);
        if (!ret || !strcmp(ret, str)) break;
    }
    return ret ? 1 : 0;
}

int gtk_string_list_index_of(GtkStringList *self, const char *str) {
    for (int i = 0;; ++i) {
        const char *tmp = gtk_string_list_get_string(self, i);
        if (!tmp) return -1;
        if (!strcmp(tmp, str)) return i;
    }
}