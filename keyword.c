/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "keyword.h"
#include "lex.h"

Tkeyword keyword_array[] = {
    {.keyword = "int", .token_type = TOKEN_KW_INT},
    {.keyword = "double", .token_type = TOKEN_KW_DOUBLE},
    {.keyword = "string", .token_type = TOKEN_KW_STRING},
    {.keyword = "if", .token_type = TOKEN_KW_IF},
    {.keyword = "else", .token_type = TOKEN_KW_ELSE},
    {.keyword = "while", .token_type = TOKEN_KW_WHILE},
    {.keyword = "read", .token_type = TOKEN_KW_READ},
    {.keyword = "write", .token_type = TOKEN_KW_WRITE},
    {.keyword = "sort", .token_type = TOKEN_KW_SORT},
    {.keyword = "", .token_type = 0}
};
const int keyword_array_size =
    sizeof(keyword_array) / sizeof(keyword_array[0]);

/**
 * Returns expression as keyword or identifier
 *
 * @param array Array of keywords
 * @param expr Expression to be identified as a keyword or identifier
 */
int is_keyword(char *expr)
{
    assert(expr != NULL);

    int i = 0;

    // Adding indent as a last item of array.
    keyword_array[keyword_array_size - 1].keyword = expr;

    // Searching expr.
    i = 0;
    while (strcmp(keyword_array[i].keyword, expr) != 0) {
	i++;
    }

    //Determinig expr as keyword or idenifier
    if (i == keyword_array_size - 1) {
	// IDENTIFIER
	return 0;
    } else {
	// KEYWORD
	return keyword_array[i].token_type;
    }
}
