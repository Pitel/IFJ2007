/*
 * Implementace interpretu imperativniho jazyka IFJ07
 *
 * Autori:
 *  xfilom00 -- Filo Martin
 *  xkalab00 -- Kalab Jan
 *  xkobli00 -- Koblizek Ondrej
 *  xnovyo01 -- Novy Ondrej
 */


#include <stdio.h>

#include "str.h"

#ifndef LEX_H
#define LEX_H


enum lstates {
    LSTATE_INIT,
    LSTATE_END,
    LSTATE_SYMBOL,
    LSTATE_INT,
    LSTATE_DOUBLE,
    LSTATE_DOUBLE_P,
    LSTATE_DOUBLE_EXP,
    LSTATE_DOUBLE_EXP_P,
    LSTATE_DOUBLE_EXP_SIGN,
    LSTATE_DOUBLE_EXP_SIGN_P,
    LSTATE_COMPARE_LESS,
    LSTATE_COMPARE_MORE,
    LSTATE_COMPARE_EQUAL,
    LSTATE_COMPARE_NOT,
    LSTATE_SLASH,
    LSTATE_QUOTE,
    LSTATE_EQUOTE,
    LSTATE_COMMENT,
    LSTATE_COMMENT_END,
};

enum token_types {
    TOKEN_SYMBOL = 1,
    TOKEN_INT,
    TOKEN_DOUBLE,
    TOKEN_STRING,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_OVER,
    TOKEN_ASSIGN_OP,
    TOKEN_DOLAR,
    TOKEN_BLOCK_BEGIN,
    TOKEN_BLOCK_END,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_MORE,
    TOKEN_MORE_EQUAL,
    TOKEN_L_BRACKET,
    TOKEN_R_BRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_EXCLAM,
    TOKEN_KW_INT,
    TOKEN_KW_DOUBLE,
    TOKEN_KW_STRING,
    TOKEN_KW_IF,
    TOKEN_KW_ELSE,
    TOKEN_KW_WHILE,
    TOKEN_KW_READ,
    TOKEN_KW_WRITE,
    TOKEN_KW_SORT,
    TOKEN_EOF,
    TOKEN_SYNEXP_E,
};

typedef struct Tlex_tokenS {
    Tstr lexeme;
    size_t token_type;
} Tlex_token;

void lex(FILE *stream, Tlex_token *token);
void token_free(Tlex_token *token);


#endif
