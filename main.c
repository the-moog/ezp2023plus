#include "config.h"
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <adwaita.h>
#include <ezp_prog.h>
#include <ezp_errors.h>

#include "windows/chips-editor/chips-editor.h"
#include "windows/main/main-window.h"
#include "chips-data-repository.h"

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
file_chooser_cb(GObject *source_object, GAsyncResult *res, gpointer data) {
    GError *error = NULL;
    AdwDialog *dlg;
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source_object), res, &error);
    if (error) {
        if (!g_error_matches(error, g_quark_try_string("gtk-dialog-error-quark"), GTK_DIALOG_ERROR_DISMISSED)) {
            dlg = adw_alert_dialog_new(gettext("Error"), error->message);
            adw_alert_dialog_add_response(ADW_ALERT_DIALOG(dlg), "OK", gettext("OK"));
            adw_dialog_present(dlg, GTK_WIDGET(data));
        }
        g_error_free(error);
        return;
    }

    char *path = g_file_get_path(file);
    int ret = chips_data_repository_import(repo, path);
    const char *error_msg;
    switch (ret) {
        case EZP_ERROR_IO:
            error_msg = "Can't read chips data: io error";
            break;
        case EZP_ERROR_INVALID_FILE:
            error_msg = "Can't read chips data: invalid file";
            break;
        case 0:
            break;
        default:
            printf("Can't read chips data: unknown error %d\n", ret);
            error_msg = "Can't read chips data: unknown error";
            break;
    }

    dlg = adw_alert_dialog_new(!ret ? gettext("Success") : gettext("Error"),
                               !ret ? "Data imported successfully" : error_msg);
    adw_alert_dialog_add_response(ADW_ALERT_DIALOG(dlg), "OK", gettext("OK"));
    adw_dialog_present(dlg, GTK_WIDGET(data));

    g_free(path);
    g_object_unref(file);
}

static void
import(G_GNUC_UNUSED GSimpleAction *action, G_GNUC_UNUSED GVariant *state, gpointer user_data) {
    GtkApplication *app = GTK_APPLICATION (user_data);
    GtkWindow *window = gtk_application_get_active_window(app);

    GtkFileDialog *dlg = gtk_file_dialog_new();
    gtk_file_dialog_open(dlg, window, NULL, file_chooser_cb, NULL);
}

static void
show_main_window(GtkApplication *app) {
    WindowMain *window = window_main_new(app, repo);
    gtk_window_present(GTK_WINDOW (window));
}

char *prepare_data_file() {
    GFile *user_file = g_file_new_build_filename(g_get_user_data_dir(), PROJECT_NAME, "chips.dat", NULL);
    char *path = g_file_get_path(user_file);
    if (!g_file_query_exists(user_file, NULL)) {
        GError *error = NULL;
        GFile *user_data_dir = g_file_get_parent(user_file);
        if (!g_file_query_exists(user_data_dir, NULL)) {
            g_file_make_directory_with_parents(user_data_dir, NULL, &error);
        }
        if (error) { //TODO: show this error in GUI
            g_error("Failed to create user data directory: %s\n", error->message);
        }

        const gchar * const* dirs = g_get_system_data_dirs();
        for (const gchar * const* dir = dirs; *dir; dir++) {
            GFile *system_file = g_file_new_build_filename(*dir, PROJECT_NAME, "chips.dat", NULL);
            if (g_file_query_exists(system_file, NULL)) {
                gboolean res = g_file_copy(system_file, user_file, G_FILE_COPY_NONE, NULL, NULL, NULL, &error);
                if (error) {
                    printf("%s\n", error->message);
                    g_error_free(error);
                }
                if (!res) {
                    printf("Can't copy chips datafile\n");
                }
                g_object_unref(system_file);
                break;
            }
            g_object_unref(system_file);
        }
    }
    g_object_unref(user_file);
    return path;
}

int
main(int argc, char **argv) {
    int status = ezp_init();
    if (status) {
        printf("ezp_init failed\n");
        return status;
    }
    const char *locale_dir_override = g_getenv("EZP_LOCALE_DIR");
    bindtextdomain(TRANSLATION_DOMAIN, locale_dir_override ? locale_dir_override : LOCALE_DIR);
    bind_textdomain_codeset(TRANSLATION_DOMAIN, "UTF-8");
    textdomain(TRANSLATION_DOMAIN);

    char *path = prepare_data_file();
    repo = chips_data_repository_new(path);
    int ret = chips_data_repository_read(repo);
    if (ret) {
        printf("Data file path: %s\n", path);
        switch (ret) {
            case EZP_ERROR_IO:
                printf("Can't read chips data: io error\n");
                break;
            case EZP_ERROR_INVALID_FILE:
                printf("Can't read chips data: invalid file\n");
                break;
            default:
                printf("Can't read chips data: unknown error %d\n", ret);
                break;
        }
    }

    AdwApplication *app;
    static GActionEntry app_entries[] = {
            {"inspector",    show_inspector,    NULL, NULL, NULL, {0}},
            {"chips_editor", show_chips_editor, NULL, NULL, NULL, {0}},
            {"about",        show_about,        NULL, NULL, NULL, {0}},
            {"import",       import,            NULL, NULL, NULL, {0}},
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
