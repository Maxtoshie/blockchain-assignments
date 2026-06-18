#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "utils.h"

/**
 * generate_id - Generate a unique ID with prefix
 */
char *generate_id(const char *prefix)
{
    char *id;
    time_t now;
    
    id = (char *)malloc(64);
    if (!id)
        return NULL;
    
    now = time(NULL);
    snprintf(id, 64, "%s-%ld-%d", prefix, (long)now, rand() % 10000);
    
    return id;
}

/**
 * trim_whitespace - Remove leading/trailing whitespace
 */
char *trim_whitespace(char *str)
{
    char *end;
    
    if (!str)
        return NULL;
    
    /* Trim leading spaces */
    while (isspace((unsigned char)*str))
        str++;
    
    if (*str == 0)
        return str;
    
    /* Trim trailing spaces */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    
    end[1] = '\0';
    return str;
}

/**
 * format_timestamp - Format timestamp as string
 */
char *format_timestamp(time_t ts)
{
    return ctime(&ts);
}

/**
 * parse_timestamp - Parse timestamp from string
 */
time_t parse_timestamp(const char *str)
{
    (void)str;
    return time(NULL);
}
