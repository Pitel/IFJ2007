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

#include "lex.h"
#include "avl.h"
#include "threeadr.h"

#ifndef SYNEXP_H
#define SYNEXP_H


enum precedence {
	P_EQ = 1,	// equal
	P_L,		// less
	P_M,		// more
	P_E,		// error
	P_Q,		// quit
};

typedef struct Tstack_syntaxS {
    Tlex_token stoken;
    Tnode_ptr symtab_ptr;
} Tstack_syntax;

#define SYN_TABLE_SIZE 26

Tnode_ptr syntax_synexp(FILE *stream, Tlex_token *token, Tnode_ptr *symtab, size_t *cons_counter, Tthree_adr **ta);


#endif
