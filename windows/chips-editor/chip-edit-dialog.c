#include "chip-edit-dialog.h"
#include "utilities.h"
#include <glib/gi18n.h>

struct _DialogChipsEdit {
    AdwDialog parent_instance;

    AdwHeaderBar *header_bar;
    GtkEntry *type_selector;
    GtkEntry *manuf_selector;
    GtkEntry *name_selector;
    GtkEntry *chip_id_selector;
    GtkSpinButton *flash_size_selector;
    GtkSpinButton *flash_page_size_selector;
    GtkDropDown *voltage_selector;
    GtkDropDown *class_selector;
    GtkDropDown *algorithm_selector;
    GtkSpinButton *delay_selector;
    GtkSpinButton *eeprom_size_selector;
    GtkSpinButton *eeprom_page_size_selector;
    GtkSpinButton *extend_selector;
    GtkButton *prev_btn;
    GtkButton *save_btn;
    GtkButton *next_btn;

    ChipsDataRepository *repo;
    guint current_index;
    gboolean has_unsaved_changes;
};

G_DEFINE_FINAL_TYPE(DialogChipsEdit, dialog_chips_edit, ADW_TYPE_DIALOG)

static void
setup_algorithm_selector(GtkDropDown *selector, uint8_t clazz, uint8_t algorithm) {
    if (clazz > 4) {
        GtkStringList *l = gtk_string_list_new(NULL);
        gtk_string_list_append(l, gettext("Corrupted data"));
        gtk_drop_down_set_model(selector, G_LIST_MODEL(l));
        return;
    }

    GtkStringList *l = gtk_string_list_new(algorithms_for[clazz]);
    gtk_drop_down_set_model(selector, G_LIST_MODEL(l));
    if (clazz == EEPROM_93) {
        gtk_drop_down_set_selected(selector, algorithm < 87 ? algorithm - 7 : algorithm - 82);
    } else {
        gtk_drop_down_set_selected(selector, algorithm);
    }
}

static void
chips_list_changed_cb(ChipsDataRepository *repo, chips_list *list, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    ezp_chip_data *data = &chips_data_repository_get_chips(dce->repo).data[dce->current_index];

    char full_name[48];
    strlcpy(full_name, data->name, 48);
    char *type;
    char *manuf;
    char *name;
    if (parseName(full_name, &type, &manuf, &name)) {
        strcpy(full_name, "Error!");
        type = manuf = name = full_name;
    }
    gtk_entry_set_text(dce->type_selector, type);
    gtk_entry_set_text(dce->manuf_selector, manuf);
    gtk_entry_set_text(dce->name_selector, name);

    char chip_id[11];
    snprintf(chip_id, 11, "0x%x", data->chip_id);
    gtk_entry_set_text(dce->chip_id_selector, chip_id);

    setup_algorithm_selector(dce->algorithm_selector, data->clazz, data->algorithm);

    gtk_spin_button_set_value(dce->flash_size_selector, data->flash);
    gtk_spin_button_set_value(dce->flash_page_size_selector, data->flash_page);
    gtk_drop_down_set_selected(dce->voltage_selector, data->voltage);
    gtk_drop_down_set_selected(dce->class_selector, data->clazz);
    gtk_spin_button_set_value(dce->delay_selector, data->delay);
    gtk_spin_button_set_value(dce->eeprom_size_selector, data->eeprom);
    gtk_spin_button_set_value(dce->eeprom_page_size_selector, data->eeprom_page);
    gtk_spin_button_set_value(dce->extend_selector, data->extend);

    dce->has_unsaved_changes = false;
    adw_dialog_set_title(ADW_DIALOG(user_data), gettext("Chips editor"));
}

static gboolean
chip_data_from_widgets(DialogChipsEdit *self, ezp_chip_data *data) {
    //check all that may be invalid
    const char *type = gtk_entry_get_text(self->type_selector);
//    if (strlen(type) > 15) return false;

    const char *manuf = gtk_entry_get_text(self->manuf_selector);
//    if (strlen(manuf) > 15) return false;

    const char *name = gtk_entry_get_text(self->name_selector);
//    if (strlen(name) > 15) return false;

    const char *chip_id_str = gtk_entry_get_text(self->chip_id_selector);
    if (strlen(chip_id_str) > 10) return false;
    if (!g_regex_match_simple("^0x[\\da-fA-F]+$", chip_id_str, G_REGEX_DEFAULT, G_REGEX_MATCH_DEFAULT)) return false;

    //set values
    snprintf(data->name, 48, "%s,%s,%s", type, manuf, name);
    data->chip_id = strtol(chip_id_str, NULL, 16);
    data->clazz = gtk_drop_down_get_selected(self->class_selector);

    data->algorithm = gtk_drop_down_get_selected(self->algorithm_selector);
    if (data->clazz == EEPROM_93) {
        if (data->algorithm < 5) data->algorithm += 7;
        else data->algorithm += 82;
    }

    data->flash = gtk_spin_button_get_value_as_int(self->flash_size_selector);
    data->flash_page = gtk_spin_button_get_value_as_int(self->flash_page_size_selector);
    data->voltage = gtk_drop_down_get_selected(self->voltage_selector);
    data->delay = gtk_spin_button_get_value_as_int(self->delay_selector);
    data->eeprom = gtk_spin_button_get_value_as_int(self->eeprom_size_selector);
    data->eeprom_page = gtk_spin_button_get_value_as_int(self->eeprom_page_size_selector);
    data->extend = gtk_spin_button_get_value_as_int(self->extend_selector);
    return true;
}

static void
save_btn_click_cb(GtkButton *btn, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    ezp_chip_data data;
    gboolean ret = chip_data_from_widgets(dce, &data);
    printf("chip_data_from_widgets returned %d\n", ret);
    if (ret) {
        //TODO: check return value
        chips_data_repository_edit(dce->repo, (int) dce->current_index, &data);
        if (dce->has_unsaved_changes) {
            dce->has_unsaved_changes = false;
            adw_dialog_set_title(ADW_DIALOG(user_data), gettext("Chips editor"));
        }
    }
}

static void
prev_btn_click_cb(GtkButton *btn, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    //TODO: check unsaved changes
}

static void
next_btn_click_cb(GtkButton *btn, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    //TODO: check unsaved changes
}

static void
chip_id_text_changed(GtkEditable* self, gpointer user_data) {
    const char *chip_id_str = gtk_entry_get_text(GTK_ENTRY(self));
    if (strlen(chip_id_str) > 10) {
        gtk_widget_add_css_class(GTK_WIDGET(self), "error");
    } else if (!g_regex_match_simple("^0x[\\da-fA-F]+$", chip_id_str, G_REGEX_DEFAULT, G_REGEX_MATCH_DEFAULT)) {
        gtk_widget_add_css_class(GTK_WIDGET(self), "error");
    } else {
        gtk_widget_remove_css_class(GTK_WIDGET(self), "error");
    }
}

static void
class_selection_changed_cb(GtkDropDown *self, G_GNUC_UNUSED gpointer *new_value, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    setup_algorithm_selector(dce->algorithm_selector, gtk_drop_down_get_selected(self), 0);
}

static void
any_entry_changed_cb(GtkEditable* self, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    if (!dce->has_unsaved_changes) {
        dce->has_unsaved_changes = true;
        adw_dialog_set_title(ADW_DIALOG(user_data), gettext("Chips editor (unsaved)"));
    }
}

static void
any_drop_down_selection_changed_cb(GtkDropDown *self, G_GNUC_UNUSED gpointer *new_value, gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    if (!dce->has_unsaved_changes) {
        dce->has_unsaved_changes = true;
        adw_dialog_set_title(ADW_DIALOG(user_data), gettext("Chips editor (unsaved)"));
    }
}

static void
dialog_chips_editor_realize(GtkWidget* self,gpointer user_data) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(user_data);
    ////////////ATTENTION! SHIT CODE BEGIN!//////////////////////
    //remove this fix and other related stuff when libadwaita will be fixed
    gpointer win_handle = gtk_widget_get_first_child(GTK_WIDGET(dce->header_bar));
    gpointer center_box = gtk_widget_get_first_child(GTK_WIDGET(win_handle));
    gpointer gizmo = gtk_center_box_get_end_widget(GTK_CENTER_BOX(center_box));
    gpointer box = gtk_widget_get_first_child(gizmo);
    gpointer controls = gtk_widget_get_first_child(box);
    gpointer cl_btn = gtk_widget_get_first_child(controls);
    gtk_actionable_set_action_name(cl_btn, "window.close");
    /////////////////////////////////////////////////////////////
}

static void
unsaved_alert_response_cb(AdwAlertDialog* self, gchar* response, gpointer user_data) {
    if (!strcmp(response, "discard")) {
        adw_dialog_force_close(ADW_DIALOG(user_data));
    } else if (!strcmp(response, "save")) {
        save_btn_click_cb(NULL, user_data);
        adw_dialog_force_close(ADW_DIALOG(user_data));
    }
}

static void
chip_edit_dialog_close_attempt(AdwDialog* dialog) {
    DialogChipsEdit *dce = EZP_DIALOG_CHIPS_EDIT(dialog);
    if (dce->has_unsaved_changes) {
        AdwAlertDialog *alert = ADW_ALERT_DIALOG(adw_alert_dialog_new(gettext("Save changes?"), gettext("Current chip has unsaved changes. Changes which are not saved will be permanently lost.")));

        adw_alert_dialog_add_responses(alert, "cancel", gettext("_Cancel"), "discard", gettext("_Discard"), "save", gettext("S_ave"), NULL);

        adw_alert_dialog_set_response_appearance(alert, "cancel", ADW_RESPONSE_DEFAULT);
        adw_alert_dialog_set_response_appearance(alert, "discard", ADW_RESPONSE_DESTRUCTIVE);
        adw_alert_dialog_set_response_appearance(alert, "save", ADW_RESPONSE_SUGGESTED);

        adw_alert_dialog_set_default_response(alert, "cancel");
        adw_alert_dialog_set_close_response(alert, "cancel");

        g_signal_connect(alert, "response", G_CALLBACK(unsaved_alert_response_cb), dialog);

        adw_dialog_present(ADW_DIALOG(alert), GTK_WIDGET(dialog));
    } else {
        adw_dialog_force_close(dialog);
    }
}

static void
dialog_chips_edit_class_init(DialogChipsEditClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    klass->parent_class.close_attempt = chip_edit_dialog_close_attempt;

    gtk_widget_class_set_template_from_resource(widget_class,
                                                "/dev/alexandro45/ezp2023plus/ui/windows/chips-editor/chip-edit-dialog.ui");

    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, header_bar);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, type_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, manuf_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, name_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, chip_id_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, flash_size_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, flash_page_size_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, voltage_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, class_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, algorithm_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, delay_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, eeprom_size_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, eeprom_page_size_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, extend_selector);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, prev_btn);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, save_btn);
    gtk_widget_class_bind_template_child(widget_class, DialogChipsEdit, next_btn);
}

static void
dialog_chips_edit_init(DialogChipsEdit *self) {
    gtk_widget_init_template(GTK_WIDGET (self));

    disable_scroll_for(GTK_WIDGET(self->flash_size_selector));
    disable_scroll_for(GTK_WIDGET(self->flash_page_size_selector));
    disable_scroll_for(GTK_WIDGET(self->delay_selector));
    disable_scroll_for(GTK_WIDGET(self->eeprom_size_selector));
    disable_scroll_for(GTK_WIDGET(self->eeprom_page_size_selector));
    disable_scroll_for(GTK_WIDGET(self->extend_selector));

    g_signal_connect_object(self->save_btn, "clicked", G_CALLBACK(save_btn_click_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->prev_btn, "clicked", G_CALLBACK(prev_btn_click_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->next_btn, "clicked", G_CALLBACK(next_btn_click_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->chip_id_selector, "changed", G_CALLBACK(chip_id_text_changed), NULL, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->class_selector, "notify::selected-item", G_CALLBACK(class_selection_changed_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(self->type_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->manuf_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->name_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->chip_id_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->flash_size_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->flash_page_size_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->delay_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->eeprom_size_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->eeprom_page_size_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->extend_selector, "changed", G_CALLBACK(any_entry_changed_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(self->voltage_selector, "notify::selected-item", G_CALLBACK(any_drop_down_selection_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->class_selector, "notify::selected-item", G_CALLBACK(any_drop_down_selection_changed_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->algorithm_selector, "notify::selected-item", G_CALLBACK(any_drop_down_selection_changed_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(self, "realize", G_CALLBACK(dialog_chips_editor_realize), self, G_CONNECT_DEFAULT);
}

DialogChipsEdit *
dialog_chips_edit_new(ChipsDataRepository *repo, guint open_index) {
    DialogChipsEdit *dlg = g_object_new(EZP_TYPE_DIALOG_CHIPS_EDIT, NULL);

    dlg->repo = repo;
    dlg->current_index = open_index;
    g_signal_connect_object(dlg->repo, "chips-list", G_CALLBACK(chips_list_changed_cb), dlg, G_CONNECT_DEFAULT);
    chips_list list = chips_data_repository_get_chips(repo);
    chips_list_changed_cb(NULL, &list, dlg);

    return dlg;
}
