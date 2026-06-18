#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <time.h>

char *generate_id(const char *prefix);
char *trim_whitespace(char *str);
char *format_timestamp(time_t ts);
time_t parse_timestamp(const char *str);

#endif /* UTILS_H */
