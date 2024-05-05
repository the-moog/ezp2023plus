#include "list-row.h"
#include <stdio.h>
#include "utilities.h"

struct _ChipsEditorListRow {
    GObject parent_instance;
    char flash_type[48];
    char manufacturer[48];
    char name[48];
    uint32_t chip_id;
    uint32_t flash;
    uint16_t flash_page;
    uint8_t clazz;
    uint8_t algorithm;
    uint16_t delay;
    uint16_t extend;
    uint16_t eeprom;
    uint8_t eeprom_page;
    uint8_t voltage;
};

G_DEFINE_FINAL_TYPE(ChipsEditorListRow, chips_editor_list_row, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_FLASH_TYPE,
    PROP_MANUFACTURER,
    PROP_NAME,
    PROP_CHIP_ID,
    PROP_VOLTAGE,
    PROP_FLASH_SIZE,
    PROP_FLASH_PAGE,
    PROP_FLASH_CLASS,
    PROP_ALGORITHM,
    PROP_DELAY,
    PROP_EEPROM_SIZE,
    PROP_EEPROM_PAGE,
    PROP_EXTEND,
    LAST_PROP,
};

static GParamSpec *props[LAST_PROP];

static void
chips_editor_list_row_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
    ChipsEditorListRow *self = CHIPS_EDITOR_LIST_ROW(object);
    char sprintf_buffer[20];
    switch (prop_id) {
        case PROP_FLASH_TYPE:
            g_value_set_string(value, chips_editor_list_row_get_flash_type(self));
            break;
        case PROP_MANUFACTURER:
            g_value_set_string(value, chips_editor_list_row_get_manufacturer(self));
            break;
        case PROP_NAME:
            g_value_set_string(value, chips_editor_list_row_get_name(self));
            break;
        case PROP_CHIP_ID:
            sprintf(sprintf_buffer, "0x%x", self->chip_id);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_VOLTAGE:
            switch (self->voltage) {
                case VOLTAGE_1V8:
                    strcpy(sprintf_buffer, "1.8v");
                    break;
                case VOLTAGE_3V3:
                    strcpy(sprintf_buffer, "3.3v");
                    break;
                case VOLTAGE_5V:
                    strcpy(sprintf_buffer, "5v");
                    break;
            }
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_FLASH_SIZE:
            sprintf(sprintf_buffer, "%d", self->flash);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_FLASH_PAGE:
            sprintf(sprintf_buffer, "%d", self->flash_page);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_FLASH_CLASS:
            sprintf(sprintf_buffer, "%d", self->clazz);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_ALGORITHM:
            sprintf(sprintf_buffer, "%d", self->algorithm);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_DELAY:
            sprintf(sprintf_buffer, "%d", self->delay);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_EEPROM_SIZE:
            sprintf(sprintf_buffer, "%d", self->eeprom);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_EEPROM_PAGE:
            sprintf(sprintf_buffer, "%d", self->eeprom_page);
            g_value_set_string(value, sprintf_buffer);
            break;
        case PROP_EXTEND:
            sprintf(sprintf_buffer, "%d", self->extend);
            g_value_set_string(value, sprintf_buffer);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}

static void
chips_editor_list_row_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
}

static void
chips_editor_list_row_class_init(ChipsEditorListRowClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->get_property = chips_editor_list_row_get_property;
    object_class->set_property = chips_editor_list_row_set_property;

    props[PROP_FLASH_TYPE] = g_param_spec_string("flash_type", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_MANUFACTURER] = g_param_spec_string("manufacturer", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_NAME] = g_param_spec_string("name", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_CHIP_ID] = g_param_spec_string("chip_id", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_VOLTAGE] = g_param_spec_string("voltage", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_FLASH_SIZE] = g_param_spec_string("flash_size", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_FLASH_PAGE] = g_param_spec_string("flash_page", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_FLASH_CLASS] = g_param_spec_string("flash_class", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_ALGORITHM] = g_param_spec_string("algorithm", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_DELAY] = g_param_spec_string("delay", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_EEPROM_SIZE] = g_param_spec_string("eeprom_size", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_EEPROM_PAGE] = g_param_spec_string("eeprom_page", NULL, NULL, "default", G_PARAM_READABLE);
    props[PROP_EXTEND] = g_param_spec_string("extend", NULL, NULL, "default", G_PARAM_READABLE);
    g_object_class_install_properties(object_class, LAST_PROP, props);
}

static void
chips_editor_list_row_init(ChipsEditorListRow *self) {
}

ChipsEditorListRow *
chips_editor_list_row_new(const ezp_chip_data *data) {
    ChipsEditorListRow *self = g_object_new(CHIPS_EDITOR_TYPE_LIST_ROW, NULL);

    char name_copy[48];
    strlcpy(name_copy, data->name, 48);
    char *clazz;
    char *manufacturer;
    char *chip_name;
    if (parseName(name_copy, &clazz, &manufacturer, &chip_name) == 0) {
        chips_editor_list_row_set_flash_type(self, clazz);
        chips_editor_list_row_set_manufacturer(self, manufacturer);
        chips_editor_list_row_set_name(self, chip_name);
    } else {
        chips_editor_list_row_set_flash_type(self, "parsing error");
    }
    self->chip_id = data->chip_id;
    self->flash = data->flash;
    self->flash_page = data->flash_page;
    self->clazz = data->clazz;
    self->algorithm = data->algorithm;
    self->delay = data->delay;
    self->extend = data->extend;
    self->eeprom = data->eeprom;
    self->eeprom_page = data->eeprom_page;
    self->voltage = data->voltage;

    return self;
}

void
chips_editor_list_row_set_flash_type(ChipsEditorListRow *self, const char *flash_type) {
    strncpy(self->flash_type, flash_type, 48);
    self->flash_type[47] = '\0';
}

void
chips_editor_list_row_set_manufacturer(ChipsEditorListRow *self, const char *manufacturer) {
    strncpy(self->manufacturer, manufacturer, 48);
    self->manufacturer[47] = '\0';
}

void
chips_editor_list_row_set_name(ChipsEditorListRow *self, const char *name) {
    strncpy(self->name, name, 48);
    self->name[47] = '\0';
}

const char *
chips_editor_list_row_get_flash_type(ChipsEditorListRow *self) {
    return self->flash_type;
}

const char *
chips_editor_list_row_get_manufacturer(ChipsEditorListRow *self) {
    return self->manufacturer;
}

const char *
chips_editor_list_row_get_name(ChipsEditorListRow *self) {
    return self->name;
}