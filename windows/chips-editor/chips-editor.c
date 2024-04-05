#include "chips-editor.h"
#include "list-row.h"
#include <glib/gi18n.h>
#include <ezp_chips_data_file.h>

struct _WindowChipsEditor {
    AdwWindow parent_instance;

    GtkListView *chips_list;
    GtkToggleButton *search_button;
    GtkSearchBar *chips_searchbar;
    GtkSearchEntry *chips_searchentry;
};

G_DEFINE_FINAL_TYPE (WindowChipsEditor, window_chips_editor, ADW_TYPE_WINDOW)

static void
window_chips_editor_class_init(WindowChipsEditorClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource(widget_class,
                                                "/dev/alexandro45/ezp2023plus/ui/windows/chips-editor/chips-editor.ui");

    gtk_widget_class_bind_template_child(widget_class, WindowChipsEditor, chips_list);
    gtk_widget_class_bind_template_child(widget_class, WindowChipsEditor, search_button);
    gtk_widget_class_bind_template_child(widget_class, WindowChipsEditor, chips_searchbar);
    gtk_widget_class_bind_template_child(widget_class, WindowChipsEditor, chips_searchentry);
}

static void
selection_changed_cb(GtkSingleSelection *selection_model, guint start_position, guint count, gpointer user_data) {
    ChipsEditorListRow *row_data = gtk_single_selection_get_selected_item(selection_model);
    printf("Type: %s, Manufacturer: %s, Name: %s\n", chips_editor_list_row_get_flash_type(row_data),
           chips_editor_list_row_get_manufacturer(row_data), chips_editor_list_row_get_name(row_data));
}

static void
search_text_changed_cb(GtkEditable *editable, gpointer data) {
    printf("search query: %s\n", gtk_editable_get_text(editable));
//    GsmApplication const *app = static_cast<GsmApplication * >(data);
//    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER (gtk_tree_model_sort_get_model(
//            GTK_TREE_MODEL_SORT(gtk_tree_view_get_model(
//                    GTK_TREE_VIEW(app->tree))))));
}

static void
window_chips_editor_init(WindowChipsEditor *self) {
    gtk_widget_init_template(GTK_WIDGET (self));

    GListStore *store = g_list_store_new(CHIPS_EDITOR_TYPE_LIST_ROW);
    ezp_chip_data *chips;
    int ret = ezp_chips_data_read(&chips, "/home/alexandro45/programs/EZP2023+ ver3.0/EZP2023+.Dat");
    if (ret > 0) {
        for (int i = 0; i < ret; ++i) {
            g_list_store_append(store, chips_editor_list_row_new(&chips[i]));
        }
    } else {
        printf("ret is %d\n", ret);
    }

    GtkListItemFactory *factory = gtk_builder_list_item_factory_new_from_resource(gtk_builder_cscope_new(),
                                                                                  "/dev/alexandro45/ezp2023plus/ui/windows/chips-editor/row.ui");
    GtkListItemFactory *headerFactory = gtk_builder_list_item_factory_new_from_resource(gtk_builder_cscope_new(),
                                                                                        "/dev/alexandro45/ezp2023plus/ui/windows/chips-editor/header.ui");

    GtkSingleSelection *selection = gtk_single_selection_new(G_LIST_MODEL(store));
    g_signal_connect(selection, "selection-changed", G_CALLBACK(selection_changed_cb), NULL);

    gtk_list_view_set_factory(self->chips_list, factory);
    gtk_list_view_set_model(self->chips_list, GTK_SELECTION_MODEL(selection));
    gtk_list_view_set_header_factory(self->chips_list, headerFactory);

    gtk_search_bar_set_key_capture_widget(self->chips_searchbar, GTK_WIDGET (self));
    g_signal_connect (self->chips_searchentry, "changed", G_CALLBACK(search_text_changed_cb), self);
}

WindowChipsEditor *
window_chips_editor_new(void) {
    return g_object_new(ADW_TYPE_WINDOW_CHIPS_EDITOR, NULL);
}