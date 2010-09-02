/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#ifndef __KEYWORD_H__
#define __KEYWORD_H__


typedef struct {
    char *keyword;
    int token_type;
} Tkeyword;

int is_keyword (char* expr);


#endif
