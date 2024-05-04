#pragma once

#include <stddef.h>
#include <gtk/gtk.h>

/**
 *
 * @param self
 * @return list size
 */
size_t gtk_string_list_size(GtkStringList *self);

/**
 *
 * @param self
 * @param str
 * @return 1 if list contains str 0 otherwise
 */
int gtk_string_list_contains(GtkStringList *self, const char* str);

/**
 *
 * @param self
 * @param str
 * @return index of str or -1
 */
int gtk_string_list_index_of(GtkStringList *self, const char* str);