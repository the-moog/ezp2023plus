#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "windows/chips-editor/chips-editor.h"
#include "windows/main/main-window.h"
#include "chips_data_repository.h"

ChipsDataRepository *repo;

static void
show_inspector(GSimpleAction *action, GVariant *state, gpointer user_data) {
    gtk_window_set_interactive_debugging(TRUE);
}

static void
show_chips_editor(GSimpleAction *action, GVariant *state, gpointer user_data) {
    WindowChipsEditor *chips_editor = window_chips_editor_new(repo);
    gtk_window_present(GTK_WINDOW(chips_editor));
}

static void
show_about(GSimpleAction *action, GVariant *state, gpointer user_data) {
    const char *developers[] = {
            "Alexandro45",
            NULL
    };

    GtkApplication *app = GTK_APPLICATION (user_data);
    GtkWindow *window = gtk_application_get_active_window(app);
    AdwDialog *about;

    about = adw_about_dialog_new_from_appdata("/dev/alexandro45/ezp2023plus/dev.alexandro45.ezp2023plus.metainfo.xml",
                                              NULL);
    adw_about_dialog_set_version(ADW_ABOUT_DIALOG(about), ADW_VERSION_S);
    adw_about_dialog_set_debug_info_filename(ADW_ABOUT_DIALOG(about), "adwaita-1-demo-debug-info.txt");
    adw_about_dialog_set_copyright(ADW_ABOUT_DIALOG(about), "© 2017–2022 Purism SPC");
    adw_about_dialog_set_developers(ADW_ABOUT_DIALOG(about), developers);

    adw_about_dialog_add_link(ADW_ABOUT_DIALOG(about),
                              _("_Documentation"),
                              "https://gnome.pages.gitlab.gnome.org/libadwaita/doc/main/");

    adw_dialog_present(about, GTK_WIDGET (window));
}

static void
show_main_window(GtkApplication *app) {
    MainWindow *window = main_window_new(app, repo);
    gtk_window_present(GTK_WINDOW (window));
}

int
main(int argc, char **argv) {
    repo = chips_data_repository_new("/home/alexandro45/programs/EZP2023+ ver3.0/EZP2023+.Dat");
    chips_data_repository_read(repo);

    AdwApplication *app;
    int status;
    static GActionEntry app_entries[] = {
            {"inspector",    show_inspector,    NULL, NULL, NULL},
            {"chips_editor", show_chips_editor, NULL, NULL, NULL},
            {"about",        show_about,        NULL, NULL, NULL},
    };

    app = adw_application_new("dev.alexandro45.ezp2023plus", G_APPLICATION_NON_UNIQUE);
    g_action_map_add_action_entries(G_ACTION_MAP (app),
                                    app_entries, G_N_ELEMENTS (app_entries),
                                    app);
    g_signal_connect (app, "activate", G_CALLBACK(show_main_window), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    return status;
}
