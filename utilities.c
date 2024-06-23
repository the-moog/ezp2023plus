#include "utilities.h"
#include <stdint.h>
#include <string.h>

const char *chip_types[] = {
        "SPI_FLASH",
        "EEPROM_24",
        "EEPROM_93",
        "EEPROM_25",
        "EEPROM_95"
};

const char *algorithms_for_spi[] = {
        "Not SST SPI flash",
        "SST SPI flash",
        NULL
};

const char *algorithms_for_24_eeprom[] = {
        "< 4KiB",
        "≥ 4KiB",
        NULL
};

const char *algorithms_for_93_eeprom[] = {
        "93C46 8 BITS",
        "93C57 8 BITS",
        "93C66 8 BITS",
        "93C76 8 BITS",
        "93C86 8 BITS",
        "93C46 16 BITS",
        "93C57 16 BITS",
        "93C66 16 BITS",
        "93C76 16 BITS",
        "93C86 16 BITS",
        NULL
};

const char *algorithms_for_25_95_eeprom[] = {
        "< 512B",
        "≥ 512B",
        "> 64KiB",
        NULL
};

const char *const *algorithms_for[] = {
        algorithms_for_spi,
        algorithms_for_24_eeprom,
        algorithms_for_93_eeprom,
        algorithms_for_25_95_eeprom,
        algorithms_for_25_95_eeprom
};

int //TODO: rename
parseName(char name[48], char **clazz, char **manufacturer, char **chip_name) {
    name[47] = '\0';
    char *str;
    uint8_t i = 0;
    while ((str = strtok_r(name, ",", &name)) != NULL) {
        switch (i) {
            case 0:
                *clazz = str;
                break;
            case 1:
                *manufacturer = str;
                break;
            case 2:
                *chip_name = str;
                return 0;
            default:
                return -1;
        }
        i++;
    }
    return -2;
}

void
fill_buf(char *hex_buffer) {
    for (int i = 0; i < 16; i++) {
        hex_buffer[i] = '\xa9';
    }
    for (int i = 16; i < 32; i++) {
        hex_buffer[i] = 'B';
    }
    for (int i = 32; i < 48; i++) {
        hex_buffer[i] = 'C';
    }
}

void
gtk_entry_set_text(GtkEntry *entry, const char *str) {
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), str, (int) strlen(str));
}

const char *
gtk_entry_get_text(GtkEntry *entry) {
    return gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
}

void
disable_scroll_for(GtkWidget *widget) {
    GListModel *l = gtk_widget_observe_controllers(widget);
    guint c = g_list_model_get_n_items(l);
    for (guint i = 0; i < c; ++i) {
        gpointer ptr = g_list_model_get_item(l, i);
        if (GTK_IS_EVENT_CONTROLLER_SCROLL(ptr)) {
            gtk_widget_remove_controller(widget, ptr);
            g_object_unref(ptr);
            break;
        }
    }
    g_object_unref(l);
}