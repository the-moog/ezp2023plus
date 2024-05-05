#include "main-window.h"
#include <glib/gi18n.h>
#include "ezp_chips_data_file.h"
#include "chips_data_repository.h"
#include "utilities.h"
#include "gtk_string_list_extension.h"

struct _MainWindow {
    AdwApplicationWindow parent_instance;

    GtkWidget *color_scheme_button;
    GtkDrawingArea *hex_widget;
    GtkScrollbar *scroll_bar;
    GtkButton *test_button;
    GtkButton *erase_button;
    GtkButton *read_button;
    GtkButton *write_button;
    GtkProgressBar *progress_bar;
    GtkButton *status_icon;
    GtkDropDown *flash_type_selector;
    GtkDropDown *flash_manufacturer_selector;
    GtkDropDown *flash_name_selector;

    char *hex_buffer;
    int hex_buffer_size;
    int viewer_offset;
    int lines_on_screen_count;
    double scroll;

    GTree *models_for_manufacturer_selector;
    GTree *models_for_name_selector;

    char selected_chip_type[48];
    char selected_chip_manuf[48];
    char selected_chip_name[48];

    ChipsDataRepository *repo;
};

G_DEFINE_FINAL_TYPE (MainWindow, main_window, ADW_TYPE_APPLICATION_WINDOW)

static char *
get_color_scheme_icon_name(gpointer user_data, gboolean dark) {
    return g_strdup (dark ? "light-mode-symbolic" : "dark-mode-symbolic");
}

static void
color_scheme_button_clicked_cb(MainWindow *self) {
    AdwStyleManager *manager = adw_style_manager_get_default();

    if (adw_style_manager_get_dark(manager))
        adw_style_manager_set_color_scheme(manager, ADW_COLOR_SCHEME_FORCE_LIGHT);
    else
        adw_style_manager_set_color_scheme(manager, ADW_COLOR_SCHEME_FORCE_DARK);
}

static void
notify_system_supports_color_schemes_cb(MainWindow *self) {
    AdwStyleManager *manager = adw_style_manager_get_default();
    gboolean supports = adw_style_manager_get_system_supports_color_schemes(manager);

    gtk_widget_set_visible(self->color_scheme_button, !supports);

    if (supports)
        adw_style_manager_set_color_scheme(manager, ADW_COLOR_SCHEME_DEFAULT);
}

#define BYTES_PER_LINE 16
#define GAP_POSITION ((BYTES_PER_LINE / 2) - 1)
#define FONT_SIZE 16
#define HEX_SPACING (FONT_SIZE * 1.5)
#define TEXT_SPACING (FONT_SIZE * 0.7)
#define GAP 20
#define HEX_BLOCK_X_OFFSET 70
#define TEXT_BLOCK_X_OFFSET (HEX_BLOCK_X_OFFSET + 430)

static void
hex_widget_draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    GdkRGBA color;
    MainWindow *mv = MAIN_MAIN_WINDOW(data);

    gtk_widget_get_color(GTK_WIDGET (area), &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, FONT_SIZE);

    char buffer[10];
    mv->lines_on_screen_count = height / FONT_SIZE;
    int i = mv->viewer_offset * BYTES_PER_LINE;
    for (int y = 0; y < mv->lines_on_screen_count; ++y) {

        //line numbers
        sprintf(buffer, "%04X", i & 0xffff);
        cairo_move_to(cr, 0, y * FONT_SIZE + FONT_SIZE);
        cairo_show_text(cr, buffer);

        for (int x = 0; x < BYTES_PER_LINE; ++x) {
            //hex
            sprintf(buffer, "%02X", mv->hex_buffer[i] & 0xff);
            cairo_move_to(cr, HEX_BLOCK_X_OFFSET + x * HEX_SPACING + (x > GAP_POSITION ? GAP : 0),
                          y * FONT_SIZE + FONT_SIZE);
            cairo_show_text(cr, buffer);

            i++;
            if (i == mv->hex_buffer_size) break;
        }
        if (i == mv->hex_buffer_size) break;
    }

    cairo_fill(cr);
}

static gboolean
hex_widget_scroll_cb(GtkEventControllerScroll *self, gdouble dx, gdouble dy, gpointer user_data) {
    MainWindow *mv = MAIN_MAIN_WINDOW(user_data);

    mv->scroll += dy;
    if (gtk_event_controller_scroll_get_unit(self) == GDK_SCROLL_UNIT_SURFACE) {
        if (fabs(mv->scroll) >= FONT_SIZE) {
            int prev_viewer_offset = mv->viewer_offset;

            while (fabs(mv->scroll) >= FONT_SIZE) {
                mv->viewer_offset += mv->scroll > 0 ? 1 : -1;
                if (mv->scroll > 0) mv->scroll -= FONT_SIZE;
                else if (mv->scroll < 0) mv->scroll += FONT_SIZE;
            }

            if (mv->viewer_offset < 0) mv->viewer_offset = 0;
            if (mv->viewer_offset > mv->hex_buffer_size / BYTES_PER_LINE)
                mv->viewer_offset = mv->hex_buffer_size / BYTES_PER_LINE;

            if (mv->viewer_offset != prev_viewer_offset) {
                gtk_widget_queue_draw(GTK_WIDGET(mv->hex_widget));
                gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(mv->scroll_bar), mv->viewer_offset);
            }
        }
    } else {
        int prev_viewer_offset = mv->viewer_offset;

        mv->viewer_offset += mv->scroll > 0 ? 1 : -1;
        mv->scroll = 0;

        if (mv->viewer_offset < 0) mv->viewer_offset = 0;
        if (mv->viewer_offset > mv->hex_buffer_size / BYTES_PER_LINE)
            mv->viewer_offset = mv->hex_buffer_size / BYTES_PER_LINE;

        if (mv->viewer_offset != prev_viewer_offset) {
            gtk_widget_queue_draw(GTK_WIDGET(mv->hex_widget));
            gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(mv->scroll_bar), mv->viewer_offset);
        }
    }
    return TRUE;
}

static void
hex_widget_scroll_begin_cb(GtkEventControllerScroll *self, gpointer user_data) {
    MAIN_MAIN_WINDOW(user_data)->scroll = 0;
}

static void
scroll_bar_value_changed_cb(GtkAdjustment *adjustment, gpointer user_data) {
    MainWindow *mv = MAIN_MAIN_WINDOW(user_data);

    int prev_viewer_offset = mv->viewer_offset;
    mv->viewer_offset = (int) gtk_adjustment_get_value(adjustment);

    if (prev_viewer_offset != mv->viewer_offset) {
        MAIN_MAIN_WINDOW(user_data)->scroll = 0;
        gtk_widget_queue_draw(GTK_WIDGET(mv->hex_widget));
    }
}

static void
main_window_button_clicked_cb(GtkButton *self, gpointer user_data) {
    const char *name = gtk_widget_get_name(GTK_WIDGET(self));

    if (!strcmp(name, "test_button")) {
        printf("Test button clicked!\n");
    } else if (!strcmp(name, "erase_button")) {
        printf("Erase button clicked!\n");
    } else if (!strcmp(name, "read_button")) {
        printf("Read button clicked!\n");
    } else if (!strcmp(name, "write_button")) {
        printf("Write button clicked!\n");
    } else {
        g_warning("Unknown button clicked!");
    }
}

static void
load_chips_task_func(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    const char *file_path = task_data;
    ezp_chip_data *data;
    int count = ezp_chips_data_read(&data, file_path);
    if (count >= 0) {
        g_task_return_pointer(task, data, NULL);
    } else {
        g_task_return_error(task, g_error_new(g_quark_from_string("quark?"), count, "Error reading chips file: %s\n",
                                              file_path));
    }
}

static void
load_chips_task_result_cb(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    ezp_chip_data *ptr = g_async_result_get_user_data(res);
    printf("Done: %s\n", res);
}

static void load_chips_task_start() {
    GTask *task = g_task_new(NULL, NULL, load_chips_task_result_cb, NULL);
    g_task_set_task_data(task, "/home/alexandro45/programs/EZP2023+ ver3.0/EZP2023+.Dat", NULL);
    g_task_run_in_thread(task, load_chips_task_func);
    g_object_unref(task);
}

static gint
compare_strings(gconstpointer a, gconstpointer b) {
    if (a == NULL && b == NULL) return 0;
    if (a == NULL || b == NULL) return -1;
    return strcmp(a, b);
}

static void
append_string_to_string_list(gpointer data, gpointer user_data) {
    gtk_string_list_append(GTK_STRING_LIST(user_data), data);
}

static gint
string_key_compare(gconstpointer a, gconstpointer b, gpointer user_data) {
    if (!a || !b) g_error("a or b is NULL");
    return strcmp(a, b);
}

static void
destroy_string(gpointer data) {
    free(data);
}

static void
destroy_strings_g_list(gpointer data) {
    g_list_free_full(data, destroy_string);
}

static void
chips_list_changed_cb(ChipsDataRepository *repo, chips_list *data, gpointer user_data) {
    ezp_chip_data *chips = data->data;
    MainWindow *mv = MAIN_MAIN_WINDOW(user_data);

    // STEP 1 - compute dropdown items
    // at first, we need to get list of types
    // then list of manufacturers for each type
    // then list of names for each manufacturer
    //
    // A GtkStringList is needed to dringList andisplay data in a GtkDropDown, but it
    // has not any "find_*" methods which we need to insert a new items
    // without duplicates. So, at first we need to use a GList, then
    // copy items to GtkStringList and delete(free) GList.
    GList *types = NULL;
    GTree *manufs = g_tree_new_full(string_key_compare, NULL, destroy_string,
                                    destroy_strings_g_list); //key is "type". stores lists with manufacturers for each type
    GTree *names = g_tree_new_full(string_key_compare, NULL, destroy_string,
                                   destroy_strings_g_list); //key is "type,manuf" stores lists with names for each manufacturer

    for (int i = 0; i < data->length; ++i) {
        char nm_cp[48];
        strlcpy(nm_cp, chips[i].name, 48);

        char *type;
        char *manuf;
        char *name;
        // replaces all comas in nm_cp by \0
        if (parseName(nm_cp, &type, &manuf, &name)) g_error("parseName");

        if (g_list_find_custom(types, type, compare_strings) == NULL) { // avoiding duplicates
            types = g_list_append(types, strdup(type));
        }

        gpointer ptr = g_tree_lookup(manufs, type);
        if (!ptr) {
            g_tree_insert(manufs, strdup(type), g_list_append(NULL, strdup(manuf)));
        } else {
            if (g_list_find_custom(ptr, manuf, compare_strings) == NULL) { // avoiding duplicates
                g_list_append(ptr, strdup(manuf));
            }
        }

        *(manuf - 1) = ','; //set comma between type and manuf instead of \0. now type is "type,manuf"

        ptr = g_tree_lookup(names, type);
        if (!ptr) {
            g_tree_insert(names, strdup(type), g_list_append(NULL, strdup(name)));
        } else {
            g_list_append(ptr, strdup(name));
        }
    }

    // copy manufacturers and free lists
    if (mv->models_for_manufacturer_selector != NULL) g_tree_destroy(mv->models_for_manufacturer_selector);
    mv->models_for_manufacturer_selector = g_tree_new_full(string_key_compare, NULL, destroy_string, g_object_unref);
    GTreeNode *node = g_tree_node_first(manufs);
    while (node) {
        GList *list = g_tree_node_value(node);
        GtkStringList *string_list = gtk_string_list_new(NULL);
        g_list_foreach(list, append_string_to_string_list, string_list);

        g_tree_insert(mv->models_for_manufacturer_selector, strdup(g_tree_node_key(node)), string_list);

        node = g_tree_node_next(node);
    }
    g_tree_destroy(manufs);

    // copy names and free lists
    if (mv->models_for_name_selector != NULL) g_tree_destroy(mv->models_for_name_selector);
    mv->models_for_name_selector = g_tree_new_full(string_key_compare, NULL, destroy_string, g_object_unref);
    node = g_tree_node_first(names);
    while (node) {
        GList *list = g_tree_node_value(node);
        GtkStringList *string_list = gtk_string_list_new(NULL);
        g_list_foreach(list, append_string_to_string_list, string_list);

        g_tree_insert(mv->models_for_name_selector, strdup(g_tree_node_key(node)), string_list);

        node = g_tree_node_next(node);
    }
    g_tree_destroy(names);

    // copy types and free temp list
    GtkStringList *types_model = gtk_string_list_new(NULL);
    g_list_foreach(types, append_string_to_string_list, types_model);
    destroy_strings_g_list(types);

    // STEP 2 - try to select previously selected items
    // find positions of previously selected items
    // set models for dropdowns
    // set selected items by its position

    // find position of previously selected type item
    int t_pos = gtk_string_list_index_of(types_model, mv->selected_chip_type);

    // find model for dropdown and position of previously selected manuf item
    GtkStringList *manufs_model = g_tree_lookup(mv->models_for_manufacturer_selector, mv->selected_chip_type);
    int m_pos = manufs_model ? gtk_string_list_index_of(manufs_model, mv->selected_chip_manuf) : -1;

    // find model for dropdown and position of previously selected name item
    char key[48];
    strlcpy(key, mv->selected_chip_type, 48);
    strlcat(key, ",", 48);
    strlcat(key, mv->selected_chip_manuf, 48);

    GtkStringList *names_model = g_tree_lookup(mv->models_for_name_selector, key);
    int n_pos = names_model ? gtk_string_list_index_of(names_model, mv->selected_chip_name) : -1;

    //set models
    gtk_drop_down_set_model(mv->flash_type_selector, G_LIST_MODEL(types_model));
    if (manufs_model) gtk_drop_down_set_model(mv->flash_manufacturer_selector, G_LIST_MODEL(manufs_model));
    if (names_model) gtk_drop_down_set_model(mv->flash_name_selector, G_LIST_MODEL(names_model));

    //set positions
    if (t_pos >= 0) gtk_drop_down_set_selected(mv->flash_type_selector, t_pos);
    if (m_pos >= 0) gtk_drop_down_set_selected(mv->flash_manufacturer_selector, m_pos);
    if (n_pos >= 0) gtk_drop_down_set_selected(mv->flash_name_selector, n_pos);
}

static void
dropdown_selected_item_changed_cb(GtkDropDown *self, gpointer *new_value, gpointer user_data) {
    // warning: dropdown_selected_item_changed_cb is called multiple times during filling dropdowns with data, so we
    // should not use this callback to do any actions. we just save selected strings and then use them when user clicks on the button
    const char *name = gtk_widget_get_name(GTK_WIDGET(self));
    MainWindow *mv = MAIN_MAIN_WINDOW(user_data);

    if (!strcmp(name, "flash_type_selector")) {
        GtkStringObject *selected_type = gtk_drop_down_get_selected_item(self);
        gtk_drop_down_set_model(mv->flash_manufacturer_selector,
                                g_tree_lookup(mv->models_for_manufacturer_selector,
                                              gtk_string_object_get_string(selected_type)));
    } else if (!strcmp(name, "flash_manufacturer_selector")) {
        GtkStringObject *selected_type = gtk_drop_down_get_selected_item(mv->flash_type_selector);
        GtkStringObject *selected_manuf = gtk_drop_down_get_selected_item(self);
        char key[48];
        strlcpy(key, gtk_string_object_get_string(selected_type), 48);
        strlcat(key, ",", 48);
        strlcat(key, gtk_string_object_get_string(selected_manuf), 48);

        gtk_drop_down_set_model(mv->flash_name_selector, g_tree_lookup(mv->models_for_name_selector, key));
    } else if (!strcmp(name, "flash_name_selector")) {
        GtkStringObject *selected_type = gtk_drop_down_get_selected_item(mv->flash_type_selector);
        GtkStringObject *selected_manuf = gtk_drop_down_get_selected_item(mv->flash_manufacturer_selector);
        GtkStringObject *selected_name = gtk_drop_down_get_selected_item(self);

        strlcpy(mv->selected_chip_type, gtk_string_object_get_string(selected_type), 48);
        strlcpy(mv->selected_chip_manuf, gtk_string_object_get_string(selected_manuf), 48);
        strlcpy(mv->selected_chip_name, gtk_string_object_get_string(selected_name), 48);

        printf("Flash selected: %s,%s,%s\n", mv->selected_chip_type, mv->selected_chip_manuf, mv->selected_chip_name);
    } else {
        g_warning("Unknown selector!");
    }
}

static void
main_window_finalize(GObject *gobject) {
    MainWindow *mv = MAIN_MAIN_WINDOW(gobject);
    g_free(mv->hex_buffer);
    if (mv->models_for_manufacturer_selector != NULL) g_tree_destroy(mv->models_for_manufacturer_selector);
    if (mv->models_for_name_selector != NULL) g_tree_destroy(mv->models_for_name_selector);
    G_OBJECT_CLASS (main_window_parent_class)->finalize(gobject);
}

static void
main_window_class_init(MainWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = main_window_finalize;

    gtk_widget_class_add_binding_action(widget_class, GDK_KEY_q, GDK_CONTROL_MASK, "window.close", NULL);

    gtk_widget_class_set_template_from_resource(widget_class,
                                                "/dev/alexandro45/ezp2023plus/ui/windows/main/main-window.ui");
    gtk_widget_class_bind_template_child(widget_class, MainWindow, color_scheme_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, hex_widget);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, scroll_bar);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, test_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, erase_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, read_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, write_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, progress_bar);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, status_icon);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, flash_type_selector);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, flash_manufacturer_selector);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, flash_name_selector);

    gtk_widget_class_bind_template_callback(widget_class, get_color_scheme_icon_name);
    gtk_widget_class_bind_template_callback(widget_class, color_scheme_button_clicked_cb);
}

static void
main_window_init(MainWindow *self) {
    AdwStyleManager *manager = adw_style_manager_get_default();

    gtk_widget_init_template(GTK_WIDGET (self));

    g_signal_connect_object(manager,
                            "notify::system-supports-color-schemes",
                            G_CALLBACK (notify_system_supports_color_schemes_cb),
                            self,
                            G_CONNECT_SWAPPED);

    notify_system_supports_color_schemes_cb(self);

    self->hex_buffer_size = 1001;
    self->hex_buffer = g_malloc(self->hex_buffer_size);
    fill_buf(self->hex_buffer);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (self->hex_widget), hex_widget_draw_function, self, NULL);

    GtkEventController *scroll = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect_object(scroll, "scroll", G_CALLBACK (hex_widget_scroll_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(scroll, "scroll-begin", G_CALLBACK (hex_widget_scroll_begin_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(self->hex_widget), GTK_EVENT_CONTROLLER (scroll));

    GtkAdjustment *scroll_adj = gtk_scrollbar_get_adjustment(self->scroll_bar);
    gtk_adjustment_set_upper(scroll_adj, (int) (self->hex_buffer_size / BYTES_PER_LINE) +
                                         (self->hex_buffer_size % BYTES_PER_LINE == 0 ? 0 : 1));
    g_signal_connect_object(scroll_adj, "value-changed", G_CALLBACK (scroll_bar_value_changed_cb), self,
                            G_CONNECT_DEFAULT);

    g_signal_connect_object(self->test_button, "clicked", G_CALLBACK (main_window_button_clicked_cb), self,
                            G_CONNECT_DEFAULT);
    g_signal_connect_object(self->erase_button, "clicked", G_CALLBACK (main_window_button_clicked_cb), self,
                            G_CONNECT_DEFAULT);
    g_signal_connect_object(self->read_button, "clicked", G_CALLBACK (main_window_button_clicked_cb), self,
                            G_CONNECT_DEFAULT);
    g_signal_connect_object(self->write_button, "clicked", G_CALLBACK (main_window_button_clicked_cb), self,
                            G_CONNECT_DEFAULT);

    gtk_progress_bar_set_fraction(self->progress_bar, 0.2);
    gtk_progress_bar_set_text(self->progress_bar, "Reading...");
    gtk_progress_bar_set_show_text(self->progress_bar, TRUE);

    gtk_button_set_icon_name(self->status_icon, "status-error");

    g_signal_connect_object(self->flash_type_selector, "notify::selected-item",
                            G_CALLBACK (dropdown_selected_item_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->flash_manufacturer_selector, "notify::selected-item",
                            G_CALLBACK (dropdown_selected_item_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->flash_name_selector, "notify::selected-item",
                            G_CALLBACK (dropdown_selected_item_changed_cb), self, G_CONNECT_DEFAULT);
}

MainWindow *
main_window_new(GtkApplication *application, ChipsDataRepository *repo) {
    MainWindow *mv = g_object_new(MAIN_TYPE_MAIN_WINDOW, "application", application, NULL);
    main_window_set_repo(mv, repo);
    return mv;
}

void
main_window_set_repo(MainWindow *self, ChipsDataRepository *repo) {
    self->repo = repo;
    g_signal_connect_object(self->repo, "chips-list", G_CALLBACK(chips_list_changed_cb), self, G_CONNECT_DEFAULT);
}