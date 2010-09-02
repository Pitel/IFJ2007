/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <string.h>

#ifndef STR_H
#define STR_H


#define STR_DEFAULT_SIZE 16

typedef struct TstrS {
    char *data;
    size_t len;
    size_t size;
} Tstr;

extern void str_init(Tstr *str);
extern void str_free(Tstr *str);
inline extern char *str_c_str(Tstr *str);
inline extern void str_strcatCh(Tstr *str, char ch);
inline extern void str_strcatChA(Tstr *str, char *ch);
inline extern void str_strcatStr(Tstr *str, Tstr *str2);
extern void str_shrink(Tstr *str);
inline extern size_t str_strlen(Tstr *str);
extern int str_strcmpChA(Tstr *str, char *ch);
inline extern int str_strcmpStr(Tstr *str, Tstr *str2);


#endif
