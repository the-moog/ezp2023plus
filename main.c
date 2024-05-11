#include "config.h"
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <adwaita.h>
#include <ezp_prog.h>

#include "windows/chips-editor/chips-editor.h"
#include "windows/main/main-window.h"
#include "chips_data_repository.h"

ChipsDataRepository *repo;

static void
show_inspector(G_GNUC_UNUSED GSimpleAction *action, G_GNUC_UNUSED GVariant *state, G_GNUC_UNUSED gpointer user_data) {
    gtk_window_set_interactive_debugging(TRUE);
}

static void
show_chips_editor(G_GNUC_UNUSED GSimpleAction *action, G_GNUC_UNUSED GVariant *state,
                  G_GNUC_UNUSED gpointer user_data) {
    WindowChipsEditor *chips_editor = window_chips_editor_new(repo);
    gtk_window_present(GTK_WINDOW(chips_editor));
}

static void
show_about(G_GNUC_UNUSED GSimpleAction *action, G_GNUC_UNUSED GVariant *state, gpointer user_data) {
    const char *developers[] = {
            "Alexandro45",
            NULL
    };

    GtkApplication *app = GTK_APPLICATION (user_data);
    GtkWindow *window = gtk_application_get_active_window(app);
    AdwDialog *about;

    about = adw_about_dialog_new_from_appdata("/dev/alexandro45/ezp2023plus/dev.alexandro45.ezp2023plus.metainfo.xml",
                                              NULL);
    adw_about_dialog_set_copyright(ADW_ABOUT_DIALOG(about), "© 2024 Alexandro45");
    adw_about_dialog_set_developers(ADW_ABOUT_DIALOG(about), developers);

    adw_dialog_present(about, GTK_WIDGET (window));
}

static void
show_main_window(GtkApplication *app) {
    WindowMain *window = window_main_new(app, repo);
    gtk_window_present(GTK_WINDOW (window));
}

int
main(int argc, char **argv) {
    int status = ezp_init();
    if (status) {
        printf("ezp_init failed\n");
        return status;
    }
    bindtextdomain(TRANSLATION_DOMAIN, LOCALE_DIR);
    bind_textdomain_codeset(TRANSLATION_DOMAIN, "UTF-8");
    textdomain(TRANSLATION_DOMAIN);

    repo = chips_data_repository_new("/home/alexandro45/programs/EZP2023+ ver3.0/EZP2023+.Dat");
    chips_data_repository_read(repo);

    AdwApplication *app;
    static GActionEntry app_entries[] = {
            {"inspector",    show_inspector,    NULL, NULL, NULL, {0}},
            {"chips_editor", show_chips_editor, NULL, NULL, NULL, {0}},
            {"about",        show_about,        NULL, NULL, NULL, {0}},
    };

    app = adw_application_new("dev.alexandro45.ezp2023plus", G_APPLICATION_NON_UNIQUE);
    g_action_map_add_action_entries(G_ACTION_MAP (app),
                                    app_entries, G_N_ELEMENTS (app_entries),
                                    app);
    g_signal_connect (app, "activate", G_CALLBACK(show_main_window), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);

    g_object_unref(app);
    ezp_free();

    return status;
}
