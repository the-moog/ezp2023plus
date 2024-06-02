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

int
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