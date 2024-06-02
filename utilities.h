#pragma once

extern const char *chip_types[];

/**
 * @brief Parses a string containing device information into separate components.
 *
 * This function parses a string containing device information separated by commas
 * into separate components: class, manufacturer, and chip name.
 *
 * @param name The input string containing device information.
 * @param clazz Pointer to a character pointer where the parsed class will be stored.
 * @param manufacturer Pointer to a character pointer where the parsed manufacturer will be stored.
 * @param chip_name Pointer to a character pointer where the parsed chip name will be stored.
 *
 * @return Returns 0 upon successful parsing and storing all components,
 *         -1 if there are more components than expected, and
 *         -2 if there are fewer components than expected.
 */
int
parseName(char name[48], char **clazz, char **manufacturer, char **chip_name);

/**
 * fill buffer with 48-bytes test data
 * @param hex_buffer
 */
void
fill_buf(char *hex_buffer);