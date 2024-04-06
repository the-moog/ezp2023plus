#include "main-window.h"

#include <glib/gi18n.h>

struct _MainWindow {
    AdwApplicationWindow parent_instance;

    GtkWidget *color_scheme_button;
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

//static void
//toast_undo_cb (AdwDemoWindow *self)
//{
//  adw_demo_page_toasts_undo (self->toasts_page);
//}

static void
main_window_class_init(MainWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_add_binding_action(widget_class, GDK_KEY_q, GDK_CONTROL_MASK, "window.close", NULL);

    gtk_widget_class_set_template_from_resource(widget_class, "/dev/alexandro45/ezp2023plus/ui/windows/main/main-window.ui");
    gtk_widget_class_bind_template_child (widget_class, MainWindow, color_scheme_button);
//    gtk_widget_class_bind_template_child (widget_class, AdwDemoWindow, split_view);
//    gtk_widget_class_bind_template_child (widget_class, AdwDemoWindow, content_page);
//    gtk_widget_class_bind_template_child (widget_class, AdwDemoWindow, stack);
//  gtk_widget_class_bind_template_child (widget_class, AdwDemoWindow, toasts_page);
    gtk_widget_class_bind_template_callback (widget_class, get_color_scheme_icon_name);
    gtk_widget_class_bind_template_callback (widget_class, color_scheme_button_clicked_cb);
//    gtk_widget_class_bind_template_callback (widget_class, notify_visible_child_cb);

//  gtk_widget_class_install_action (widget_class, "toast.undo", NULL, (GtkWidgetActionActivateFunc) toast_undo_cb);
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
}

MainWindow *
main_window_new(GtkApplication *application) {
    return g_object_new(MAIN_TYPE_MAIN_WINDOW, "application", application, NULL);
}
